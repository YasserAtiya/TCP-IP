#!/usr/bin/python

#Currently not sending same number of packets per station
import pdb,sys,random,numpy,math, Queue


class Message:
   packet_id = 0
   src = 0 
   dst = 0
   pkt_size = 0
   time = 0

#Grab next message
def Next_Message(inp):
   
   mess = Message()

   #Read file
   line = inp.readline(1000)

   #Divide line into variables
   vals = line.split()
 
   #Get variables
   mess.packet_id = int(vals[0])
   mess.src = int(vals[1])
   mess.dst = int(vals[2])
   mess.pkt_size = int(vals[3])
   mess.time = int(vals[4])

   return mess

#Print message start
def Start_Printing(m): 
   output.write("Time: " + str(m.time) + ", Packet " + str(m.packet_id) + ": " + str(m.src) + " " + str(m.dst) + " start sending\n")


#Print collision message
def Collision_Printing(m): 
   output.write("Time: " + str(m.time) + ", Packet " + str(m.packet_id) + ": " + str(m.src) + " " + str(m.dst) + " start sending: collision\n")

#Print message start
def Failure_Printing(m): 
   temp = int(m.time)+int(m.pkt_size)

   output.write("Time: " + str(temp) + ", Packet " + str(m.packet_id) + ": " + str(m.src) + " " + str(m.dst) + " finish sending: failed\n")

#Print success message
def Success_Printing(m):
   temp = int(m.time)+int(m.pkt_size)

   output.write("Time: " + str(temp) + ", Packet " + str(m.packet_id) + ": " + str(m.src) + " " + str(m.dst) + " finish sending: successfully transmitted\n")

#Main
#pdb.set_trace()

inp = open('traffic', 'r+')
output = open('aloha_output.txt', 'w+')

#Read total packets as first line
total_packets = int(inp.readline().rstrip())

time = 0

#Get next message
mess = Next_Message(inp)

q = Queue.Queue()

i = 0

#loop until reach that number
while (i < total_packets):
   q.put(mess)

   #Print start printing message
   Start_Printing(mess)
   
   #Grab following Message
   if(i < total_packets-1):
      nmess = Next_Message(inp)
      i+=1 
   else:
      Success_Printing(mess)
      break

   #Detect if collision
   if nmess.time < mess.time+mess.pkt_size:

      #Grab all following messages that will collide
      while nmess.time < mess.time+(mess.pkt_size):
         q.put(nmess)
         
         
         #Print collision messages
         Collision_Printing(nmess)        
 
         #Grab next message
         nmess = Next_Message(inp)
         
         #Increment counter 
         i+=1

      for j in range(0,q.qsize()):
         #Print failure messages
         Failure_Printing(q.get())
         

   #Else Print success message
   else:
      Success_Printing(mess)
   
   #Grab next message   

   mess = nmess
   
   q = Queue.Queue()


