import socket
serverName = 'vcm-32291.vm.duke.edu'
#'vcm-19383.vm.duke.edu'
packagePort = 9999
# usernamePort = 9999
from .models import *
def sendToserver(orders):
    send_success = True
    for order in orders:
        backend_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        backend_socket.connect((serverName,packagePort))
        msg = str(order.pack_id)+'\n'
        try:
            backend_socket.send(msg.encode('utf-8'))
        except socket.error as err:
            print("Socket send error!")
        response =  backend_socket.recv(256)
        response = response.decode()
        if(response!='ACK'):
            send_success = False
    return send_success
        # components = response.split(' ')
        # if components[0] == 'received' and components[1]==str(packageID):
        #     return True
        # else:
        #     print('backend says: '+response)
        #     return False
    # except ConnectionRefusedError:
    #     return False
