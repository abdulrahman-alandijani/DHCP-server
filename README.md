To compile:
       server:
              make server
       client: 
              make client
it will create binary files for both codes

To Execute:
       server:
              ./server (port_numer)
       client:
              ./client (port_number)

To clean:
       make clean
it will remove all binary files

To Test: when running ->>
       server:
              make server
              ./server 1234
              Network address: 123.123.123.123
              Subnet number: 28
       client:
              make client
              ./client 1234
              Enter start to connecto to DHCP server: start ->> (you can enter any letter here)
       done:
              make clean