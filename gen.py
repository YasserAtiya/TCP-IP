#!/usr/bin/python

#Currently not sending same number of packets per station
import sys,random,numpy,math

#Generate Packet
def gen(seed, i, gap):

   #Gen src
   src = rand(seed, i, num_nodes/2)

   #Gen dst
   dst = rand(seed, i*i, num_nodes/2)

   #Print packet id   
   output.write(str(i) + " " + str(src) + " " + str(dst) + " ")


#Personal random number generator
def rand(seed, val, gap):
  
   #Same seed should always create same data
   if seed != -1:
      #Use modular hashing
      return (val*gap)%seed
   else:
      #Use rand function
      return random.randint(0,int(2*gap))

#Main

output = open('traffic', 'w+')

#Get number of nodes
num_nodes = int(sys.argv[1])   

#Get packet size
pkt_size = int(sys.argv[2])

#Get Offered Load
offered_load = float(sys.argv[3])

#Get packets per load
num_packets_per_node = int(sys.argv[4])

time = 0

#Get Optional Seed
seed = -1
if len(sys.argv) > 5:
   seed = int(sys.argv[5])

#Calculate gap
gap = (((pkt_size*num_nodes)/offered_load) - pkt_size)
#output.write("gap: " + str(gap) + "\n")

#Calculate first line stating number of packets IDK
total_packets = int(math.floor(num_nodes*offered_load*num_packets_per_node))

#Print total packets as first line
output.write(str(total_packets)+"\n")

#loop until reach that number
for i in range(0,int(total_packets)):

   #Generate packet
   gen(seed, i, gap)

   #Print packet size and time
   output.write(str(pkt_size) + " ")

   #Print time
   output.write(str(time) + "\n")

   #Increment time
   time+=rand(seed,i,gap)
   
