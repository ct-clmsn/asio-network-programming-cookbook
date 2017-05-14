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

ch02: clean
	$(CC) $(ALL_FLAGS) -o $(BIN)/01_FixedLengthBufferOutput $(SRC)/ch02/01_Fixed_length_buffer_output.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/02_FixedLengthBufferInput $(SRC)/ch02/02_Fixed_length_buffer_input.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/03_ExtensibleStreamOrientedBuffer $(SRC)/ch02/03_Extensible_stream_oriented_buffer.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/04_WritingToTCPSocketSync $(SRC)/ch02/04_Writing_to_TCP_socket_sync.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/05_ReadingFromTCPSocketSync $(SRC)/ch02/05_Reading_from_TCP_socket_sync.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/06_WritingToTCPSocketAsync $(SRC)/ch02/06_Writing_to_TCP_socket_async.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/07_WritingToTCPSocketAsyncEnhanced $(SRC)/ch02/07_Writing_to_TCP_socket_async_enhanced.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/08_ReadingFromTCPSocketAsync $(SRC)/ch02/08_Reading_from_TCP_socket_async.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/09_ReadingFromTCPSocketAsyncEnhanced $(SRC)/ch02/09_Reading_from_TCP_socket_async_enhanced.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/10_CancelingAsyncOperations $(SRC)/ch02/10_Canceling_async_operations.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/11_SocketShutdownClient $(SRC)/ch02/11_Socket_shutdown_client.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/12_SocketShutdownServer $(SRC)/ch02/12_Socket_shutdown_server.cpp

ch03: clean
	$(CC) $(ALL_FLAGS) -o $(BIN)/01_SyncTCPClient $(SRC)/ch03/01_Sync_tcp_client.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/02_SyncUDPClient $(SRC)/ch03/02_Sync_udp_client.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/03_AsyncTCPClient $(SRC)/ch03/03_Async_tcp_client.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/04_AsyncTCPClientMT $(SRC)/ch03/04_Async_tcp_client_mt.cpp

ch04: clean
	$(CC) $(ALL_FLAGS) -o $(BIN)/01_SyncIterativeTCPServer $(SRC)/ch04/01_Sync_iterative_tcp_server.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/02_SyncParallelTCPServer $(SRC)/ch04/02_Sync_parallel_tcp_server.cpp
	$(CC) $(ALL_FLAGS) -o $(BIN)/03_AsyncParallelTCPServer $(SRC)/ch04/03_Async_parallel_tcp_server.cpp
