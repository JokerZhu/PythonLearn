#!/usr/bin/python3.4
import tkinter as  tk

root = tk.Tk()
tk.Label(root,text = 'IP:').grid(row=0,column=0)
tk.Label(root,text = 'PORT:').grid(row = 1,column = 0)

def connectHSM():
    print('connecting...')
    pass
e_ip = tk.Entry(root)
#e.set('192.168')
e_port = tk.Entry(root)
e_ip.grid(row = 0,column = 1,padx=10,pady=5)
e_port.grid(row = 1,column = 1,padx=10,pady=5)

tk.Button(root,text = '连接',width = 10,command=connectHSM).grid(row=3,column=0,padx=10,pady=5)

e_data = tk.Entry(root,width=50)
e_data.grid(row=4,column=0,padx=10,pady=5)

root.mainloop()