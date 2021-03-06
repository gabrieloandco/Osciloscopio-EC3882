from mttkinter import mtTkinter as tk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import time
import threading
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator
LARGE_FONT= ("Verdana", 12)
import serial
import numpy as np

 
continuePlotting = False
channel1state = True
channel2state = True
channelD1state = False
channelD2state = False


 
def change_state():
    global continuePlotting
    if continuePlotting == True:
        continuePlotting = False
    else:
        continuePlotting = True

def change_channel1state():
    global channel1state
    if channel1state == True:
        channel1state = False
    else:
        channel1state = True

def change_channel2state():
    global channel2state
    if channel2state == True:
        channel2state = False
    else:
        channel2state = True

def change_channelD1state():
    global channelD1state
    if channelD1state == True:
        channelD1state = False
    else:
        channelD1state = True
    
def change_channelD2state():
    global channelD2state
    if channelD2state == True:
        channelD2state = False
    else:
        channelD2state = True

AmplitudeOptions={ '0.3 V   ':0.31, '1 V   ':1.1,'3 V   ':3.1}
#TimeOptions = {"100ms":2, "10ms":20, "1ms":150}
#TimeOptions = {"1ms":1, "2ms":2, "5ms":5}
#SampleOptions = {"1ms":150, "2ms":300, "5ms":750}
#SampleOptions = {"100ms":4, "10ms":40, "1ms":150}
LocOptions = {"0.1ms":0.01, "1ms":0.1, "10ms":1}
TimeOptions = {"0.1ms":0.1, "1ms":1, "10ms":10}
SampleOptions = {"0.1ms":2, "1ms":20, "10ms":200}

def makeFig(valch1_lower, valch1_upper, valch2_lower, valch2_upper, vald1, vald2, ax, graph, variable, var):
    ax.clear()
    ax.cla()
    ax.xaxis
    majorlocator=MultipleLocator(LocOptions[var.get()])
    ax.xaxis.set_major_locator(majorlocator)
    ax.grid(linewidth=1)
    try:
        ax.set_ylim(0,AmplitudeOptions[variable.get()])
        ax.set_ylabel('V')
        #ax.set_xlim(0,TimeOptions[var.get()])
        ax.set_xlabel('ms')
    except:
        pass

    valch1=(valch1_upper<<6)+valch1_lower
    valch2=(valch2_upper<<6)+valch2_lower

    #plt.figure(1)
    time_axis_ch1=np.linspace(0, TimeOptions[var.get()] ,num=valch1.size)
    time_axis_ch2=np.linspace(0, TimeOptions[var.get()] ,num=valch2.size)
    time_axis_d1=np.linspace(0, TimeOptions[var.get()] ,num=vald1.size)
    time_axis_d2=np.linspace(0, TimeOptions[var.get()] ,num=vald2.size)
    # plt.title('Osciloscopio Digital')
    # plt.grid(True)
    # plt.ylabel('data')
    if(channel1state):
        ax.plot(time_axis_ch1,valch1*3.0/4096, '-r', label='Channel 1')
    # plt.legend(loc='lower right')
    # #plt.figure(1)
    # plt.title('Osciloscopio Digital')
    # plt.grid(True)
    # plt.ylabel('data')
    if(channel2state):
        ax.plot(time_axis_ch2,valch2*3.0/4096, '-b', label='Channel 2')
    # plt.legend(loc='lower right')
    if(channelD1state):
        ax.plot(time_axis_d1,vald1, '-g', label='Channel D1')

    if(channelD2state):
        ax.plot(time_axis_d2,vald2, '-y', label='Channel D2')

    graph.draw()

 
def app():
    # initialise a window.
    root = tk.Tk()
    root.config(background='white')
    root.geometry("1000x700")
    
    lab = tk.Label(root, text="Osciloscopio Digital", bg = 'white').pack()
    
    fig = Figure()
    
    ax = fig.add_subplot(111)
    ax.set_xlabel("X axis")
    ax.set_ylabel("Y axis")
 
    # fig = plt.figure(1)
    # plt.ion()

    graph = FigureCanvasTkAgg(fig, master=root)
    graph.get_tk_widget().pack(side="top",fill='both',expand=True)

    variable = tk.StringVar(root)
    variable.set("3 V   ") #defaultvalue
    w=tk.OptionMenu(root, variable,"3 V   ", "1 V   ", "0.3 V   ")
    w.place(x= 300, y = 672)

    var = tk.StringVar(root)
    var.set("1ms")
    option = tk.OptionMenu(root, var,"0.1ms", "1ms", "10ms")
    option.place(x= 600, y = 672)



    def osciloscopio():
        valch1_lower = np.array([],np.dtype(int) )
        valch1_upper = np.array([],np.dtype(int) )
        valch2_lower = np.array([],np.dtype(int) )
        valch2_upper = np.array([],np.dtype(int) )
        vald1 = np.array([],np.dtype(int) )
        vald2 =  np.array([],np.dtype(int) )

        data_count=0
        cnt = 0
        point_count=0
        #create the serial port object
        port = serial.Serial('/dev/ttyUSB0', 115200, timeout=0.5)



        while continuePlotting:
            #port.write(b's') #handshake with Arduino
            if (port.inWaiting()):# if the arduino replies
                value = np.array(map(ord,port.read(8000)))# read the reply
                if cnt==0:
                    cnt = cnt+1
                    continue
                data=''

                first_mask=value[:len(value)]<128
                valch1_upper=value[np.array(np.flatnonzero(first_mask))]&63
                index1=np.flatnonzero(first_mask)+1
                valch1_lower=value[index1[:-1]]&63
                index2=np.array(np.flatnonzero(first_mask))+2
                valch2_upper=value[index2[:-2]]&63
                vald1=value[index2[:-2]] >>6
                vald1=vald1-2
                index3=np.array(np.flatnonzero(first_mask))+3
                valch2_lower=value[index3[:-3]]&63
                vald2=value[index3[:-3]] >>6
                vald2=vald2-2
                end_array=SampleOptions[var.get()]
                valch1_lower=valch1_lower[:end_array]
                #print('lower')
                #print(map(bin,valch1_lower[:10]))
                valch1_upper=valch1_upper[:end_array]
                #print(map(bin,valch1_upper[:10]))
                valch2_lower=valch2_lower[:end_array]
                valch2_upper=valch2_upper[:end_array]

                makeFig(valch1_lower, valch1_upper, valch2_lower, valch2_upper, vald1, vald2, ax, graph, variable, var)
                valch1_lower = np.array([],np.dtype(int) )
                valch1_upper = np.array([],np.dtype(int) )
                valch2_lower = np.array([],np.dtype(int) )
                valch2_upper = np.array([],np.dtype(int) )
                vald1 = np.array([],np.dtype(int) )
                vald2 =  np.array([],np.dtype(int) )

 
    def gui_handler():
        change_state()
        threading.Thread(target= osciloscopio).start()

 
    b = tk.Button(root, text="Start/Stop", command=gui_handler, bg="red", fg="white")
    b.place(x = 450, y = 20) 


    b1 = tk.Button(root, text="Channel1", command=change_channel1state, bg="blue", fg="white")
    b1.pack(side = "left")


    b2 = tk.Button(root, text="Channel2", command=change_channel2state, bg="blue", fg="white")
    b2.pack(side = "left")

    b3 = tk.Button(root, text="ChannelD1", command=change_channelD1state, bg="blue", fg="white")
    b3.pack(side = "right")

    b4 = tk.Button(root, text="ChannelD2", command=change_channelD2state, bg="blue", fg="white")
    b4.pack(side = "right")

    root.mainloop()
 
if __name__ == '__main__':
    app()