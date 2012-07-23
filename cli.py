import socket

cli = socket.socket( socket.AF_INET, socket.SOCK_STREAM )

cli.connect( ("127.0.0.1", 8888) )

while True:
	inputed = raw_input("press any key to continue...")

	if len(inputed)==0:
		print "exiting..."
		exit(0)

	cli.send(inputed)
	print cli.recv(4096)




