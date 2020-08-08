#!/bin/bash

pathRoot=$(pwd)

pathBroker="${pathRoot}/Broker/Debug/configBroker.config"
pathGamecard="${pathRoot}/GameCard/Debug/GameCard.config"
pathTeam="${pathRoot}/Team/Debug/configTeam.config"
pathGameboy="${pathRoot}/GameBoy/Debug/configGameBoy.config"

lineaAModificarBroker=0
lineaAModificarGamecard=0
lineaAModificarGameboy=0
lineaAModificarTeam=0

esTeam=false

textoIP=""
textoPuerto=""

seguir=true

if [ -z "$2" ] || [ -z "$3" ];
then
  echo "Falta ingresar ip o puerto"
  exit 0
fi

case $1 in
  broker)
    lineaAModificarBroker=1
    lineaAModificarGamecard=1
    lineaAModificarGameboy=1
    lineaAModificarTeam=1
    textoIP="IP_BROKER"
    textoPuerto="PUERTO_BROKER"
    ;;
  gamecard)
    lineaAModificarGamecard=3
    lineaAModificarGameboy=3
    textoIP="IP_GAMECARD"
    textoPuerto="PUERTO_GAMECARD"
    ;;
  team)
    lineaAModificarGameboy=5
    lineaAModificarTeam=3
    textoIP="IP_TEAM"
    textoPuerto="PUERTO_TEAM"
    esTeam=true
    ;;
  *)
    echo "No existe el módulo"
    seguir=false
esac

if [ "$seguir" = "false" ]
then
  exit 0
fi

if [ $lineaAModificarBroker -ne 0 ]
then
  sed -i "${lineaAModificarBroker}s/.*/$textoIP=$2/" $pathBroker
  sed -i "$((lineaAModificarBroker + 1))s/.*/$textoPuerto=$3/" $pathBroker
fi

if [ $lineaAModificarGamecard -ne 0 ]
then
  sed -i "${lineaAModificarGamecard}s/.*/$textoIP=$2/" $pathGamecard
  sed -i "$((lineaAModificarGamecard + 1))s/.*/$textoPuerto=$3/" $pathGamecard
fi

if [ $lineaAModificarGameboy -ne 0 ] && [ $esTeam = "true" ]
then
  ipsPuertos="["
  cantidad=$2
  iteracion=1

  while [ $iteracion -le $cantidad ]
  do
    iteracionNumero=$(expr $iteracion)
    nuevaIpVariable=$(($iteracionNumero * 2 + 1))
    nuevaIp=${!nuevaIpVariable}
    nuevoPuertoVariable=$(( $iteracionNumero * 2 + 2))
    nuevoPuerto=${!nuevoPuertoVariable}

    if [ $iteracion -ne 1 ]
    then
      ipsPuertos="$ipsPuertos,"
    fi

    ipsPuertos="$ipsPuertos$nuevaIp,$nuevoPuerto"
    iteracion=$(expr $iteracionNumero + 1)
  done

  ipsPuertos="$ipsPuertos]"

  sed -i "${lineaAModificarGameboy}s/.*/IPS_PUERTOS_TEAM=$ipsPuertos/" $pathGameboy
  sed -i "$((lineaAModificarGameboy + 1))s/.*/CANTIDAD_TEAMS=$2/" $pathGameboy
fi

if [ $lineaAModificarGameboy -ne 0 ] && [ $esTeam = "false" ]
then
  sed -i "${lineaAModificarGameboy}s/.*/$textoIP=$2/" $pathGameboy
  sed -i "$((lineaAModificarGameboy + 1))s/.*/$textoPuerto=$3/" $pathGameboy
fi

if [ $lineaAModificarTeam -ne 0 ] && [ $esTeam = "false" ]
then
  sed -i "${lineaAModificarTeam}s/.*/$textoIP=$2/" $pathTeam
  sed -i "$((lineaAModificarTeam + 1))s/.*/$textoPuerto=$3/" $pathTeam
fi

if [ $lineaAModificarTeam -ne 0 ] && [ $esTeam = "true" ] && [ $2 -eq 1 ]
then
  sed -i "${lineaAModificarTeam}s/.*/$textoIP=$3/" $pathTeam
  sed -i "$((lineaAModificarTeam + 1))s/.*/$textoPuerto=$4/" $pathTeam
fi

# Ejemplitos para cambiar configuración de Team a manopla si es necesario.
# sed -i '5s/.*/POSICIONES_ENTRENADORES=[1|3,2|3,2|2]/' configTeam.config
# sed -i '6s/.*/POKEMON_ENTRENADORES=[Pikachu, , ]/' configTeam.config
# sed -i '7s/.*/OBJETIVOS_ENTRENADORES=[Pikachu|Squirtle,Pikachu|Gengar,Squirtle|Onix]/' configTeam.config
# sed -i '12s/.*/QUANTUM=0/' configTeam.config
# sed -i '10s/.*/ALGORITMO_PLANIFICACION=FIFO/' configTeam.config