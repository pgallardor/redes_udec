# Instrucciones

1.- Compile "servidor.c"
2.- Elija el archivo a enviar y ubique en la misma carpeta
3.- Ejecuta ./servidor.out PORT_NUMBER1 NOMBRE_ARCHIVO
4.- Sin cerrarlo, entre a "Client", y compile "cliente.c".
5.- Ejecuta: ./cliente.out localhost PORT_NUMBER1 NOMBRE_ARCHIVO IP_MULTICAST PORT_NUMBER2
6.- Quien va a recibir, debe compilar "receptorMulti.c" en "/Multicast"
7.- Ejecuta ./receptor.out IP_MULTICAST PORT_NUMBER2 NOMBRE_ARCHIVO 
