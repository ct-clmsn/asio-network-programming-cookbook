CC=clang++
CFLAGS=-std=c++14 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -DNDEBUG -O3
CFLAGS_DEBUG=-std=c++14 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -DDEBUG -O0
BOOST_ASIO_INCLUDE=-I/home/jvillasante/Software/src/asio-1.10.8/include -DASIO_STANDALONE
ALL_FLAGS=$(CFLAGS_DEBUG) $(BOOST_ASIO_INCLUDE) -pthread
SRC=src
BIN=bin
RM=rm -rf
CP=cp -rf

# The Cleaner
clean:
	@$(RM) $(BIN)/*

ch01: clean
	$(CC) $(ALL_FLAGS) -o $(BIN)/01_CreatingEndpointClient $(SRC)/ch01/01_Creating_endpoint_client.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/02_CreatingEndpointServer $(SRC)/ch01/02_Creating_endpoint_server.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/03_CreatingActiveSocketTCPIPv4 $(SRC)/ch01/03_Creating_an_active_socket_TCP_IPv4.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/04_CreatingActiveSocketUDPIPv6 $(SRC)/ch01/04_Creating_an_active_socket_UDP_IPv6.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/05_CreatingPassiveSocket $(SRC)/ch01/05_Creating_a_passive_socket.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/06_ResolvingDNSNameTCP $(SRC)/ch01/06_Resolving_a_DNS_name_TCP.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/07_ResolvingDNSNameUDP $(SRC)/ch01/07_Resolving_a_DNS_name_UDP.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/08_BindingSocketTCPPassive $(SRC)/ch01/08_Binding_a_socket_TCP_passive.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/09_BindingSocketUDPActive $(SRC)/ch01/09_Binding_a_socket_UDP_active.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/10_ConnectingSocket $(SRC)/ch01/10_Connecting_a_socket.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/11_ConnectingSocketDNS $(SRC)/ch01/11_Connecting_a_socket_DNS_name.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/12_AcceptingConnection $(SRC)/ch01/12_Accepting_a_connection.cpp
