> # lorawan-tracking-system
>
> ##### Wild animals tracking - An IoT solution
>
> One of the two selected finalist of the 5th International BusITWeek "Wild Animals Tracking - An IoT solution".

# About the project

This project is a solution to help the [*Faune & Biotopes Association*](http://www.faune-biotopes.be/ "Faune & Biotopes Association homepage") to manage their tracking of wild animals using IoT devices.

This project is proposed by Faune & Biotopes Association in partnership with [EPHEC Louvain-la-Neuve](https://www.ephec.be/) and [Proximus Enco](https://enco.io/).

## Material

- LoRaWAN Rapid Development Kit
  - Sodaq Mbili card Rev6 (based on Arduino)
  - LoRa network connection chip developed by Microchip
  - Antenna
  - Battery
  - Sensors: Grove - GPS, Grove - 3-Axis Digital Accelerometer (+/- 1.5g).
- Proximus EnCo account

We basically won't use other stuff included in the LoRaWAN RDK even though some of them (TPH v2, Grove - Button (P)) could be useful for a first temporary sketch in order to familiarize with LoRaWAN.

## Faune & Biotopes Association

Faune & Biotopes is engaged in activities related to the study and the management of wildlife and habitats in a multifunctional and integrated perspective.

Faune & Biotopes considers that a good knowledge and good management of the faune and its habitats is one of the most effective approach for the conservation of the nature.

# What this project is doing

Thanks to the LoRaWAN board, the animal can be tracked.
Because we used a free account, the data is sent once an hour.

We made a webpage in order to see the content in a more conveniently manner.
A map and the complete coordinates are showing up on it.
The website can be seen (with _"demo GPS info"_ stored (more info can be added directly by the LoRaWAN Tracking System) here: <https://ephec.herokuapp.com/>

# Further implementation

- Integrate the "Grove - Light Sensor v1.2" module in order to know if the animal is underground or not and basically change also some parameters in order to not try too much sending data,
- Change the way the board is saving data in order to keep it as an array which can be send via the network LoRa and help the website to have more GPS coordinates.
