compile_programs: compile_error_handler compile_server compile_client
	@gcc errorHandler.o clientQueue.o searchRecord.o p2-server.o -o -lnsl -lpthread -o server
	@gcc errorHandler.o p2-client.o -o client
	@rm -r errorHandler.o clientQueue.o searchRecord.o p2-server.o p2-client.o
	@echo "Ejecuta el programa servidor con './server' y el programa cliente con ./client seguido de la IP del cliente"
compile_error_handler:
	@gcc -c ./utils/errorHandler.c
compile_server: compile_queue compile_search_record
	@gcc -c p2-server.c
compile_queue:
	@gcc -c ./dataStructures/clientQueue.c
compile_search_record:	
	@gcc -c ./search/searchRecord.c
compile_client:
	@gcc -c p2-client.c
