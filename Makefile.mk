compile_programs: compile_error_handler compile_server compile_client compile_many_clients
	@gcc errorHandler.o clientQueue.o searchRecord.o p2-server.o -o -lnsl -lpthread -o server
	@gcc errorHandler.o p2-client.o -o client
	@gcc errorHandler.o p2-many-clients.o -lnsl -lpthread -o many-clients
	@rm -r errorHandler.o clientQueue.o searchRecord.o p2-server.o p2-client.o p2-many-clients.o
	@echo "Ejecuta el programa servidor con './server', el programa cliente con ./client seguido de la IP del cliente, y el script de varios clientes con ./many-clients (estos clientes eventualmente finalizan la conexion, asi que por favor no oprimas control + C mientras se ejecuta). Luego de probar el servidor y los clientes, puedes consultar el registro de peticiones al servidor con 'cat output/log.out'"
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
compile_many_clients:
	@gcc -c p2-many-clients.c
