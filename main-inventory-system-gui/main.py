import bleak
import customtkinter as ctk
import pandas as pd
import json
import os.path
import time
import random
import socket
import sys
import _thread
import time
import logging
import numpy as np
from scipy import integrate
import asyncio
from threading import Thread
from bleak import BleakClient
import struct


ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("dark-blue")

#TO RESET DATA.JSON: comment out the ''' in the line below, save, and run once
'''
available_supplies = {
    1: {
        "name": "Cream 1",
        "category": "cream",
        "quantity": 132,
        "unit": "mL",
        "refillSize": 50,
        "date": "01/01/2023",
    },
    2: {
        "name": "Pill 1",
        "category": "pill",
        "quantity": 50,
        "unit": "pills",
        "refillSize": 100,
        "date": "01/01/2023",
    },
    3: {
        "name": "Bandage",
        "category": "other",
        "quantity": 360,
        "unit": "in",
        "refillSize": 25,
        "date": "01/01/2023",
    },
    4: {
        "name": "Cream 2",
        "category": "cream",
        "quantity": 50,
        "unit": "mL",
        "refillSize": 25,
        "date": "01/01/2023",
    },
    5: {
        "name": "Pill 2",
        "category": "pill",
        "quantity": 100,
        "unit": "pills",
        "refillSize": 50,
        "date": "01/01/2023",
    },
    6: {
        "name": "Cream 3",
        "category": "cream",
        "quantity": 75,
        "unit": "mL",
        "refillSize": 25,
        "date": "01/01/2023",
    },
    7: {
        "name": "Pill 3",
        "category": "pill",
        "quantity": 70,
        "unit": "pills",
        "refillSize": 50,
        "date": "01/01/2023",
    },
    8: {
        "name": "Cream 4",
        "category": "cream",
        "quantity": 90,
        "unit": "mL",
        "refillSize": 50,
        "date": "01/01/2023",
    },
    9: {
        "name": "Pill 4",
        "category": "pill",
        "unit": "pills",
        "quantity": 50,
        "refillSize": 10,
        "date": "01/01/2023",
    },
    10: {
        "name": "Pill 5",
        "category": "pill",
        "unit": "mL",
        "quantity": 60,
        "refillSize": 40,
        "date": "12/06/2023",
    },
}

# Formatting Dictionary into JSON format
js = json.dumps(available_supplies)

# json.dumps() function converts a
# Python object into a json string
js  # so we got all data in json string format here

# Create Jason File for DataBase and Write data Into File
fd = open("data.json", "w")
# it will open file into write mode if file
# does not exists then it will create file too
fd.write(js)  # writing string into file
fd.close()  # Close File After Inserting Data'''

user_id = 0
isLoggedIn = False
tableRows = 0
tableCols = 0
bandage_connected = False

connectedToBandageDevice = False

async def connecttoBandageDevice():
    def disconnectedFromBandage(disconnectArg):
        global connectedToBandageDevice
        print("disconnected from Bandage Device")
        connectedToBandageDevice = False

    async with BleakClient("58:BF:25:9C:4E:C6", disconnected_callback=disconnectedFromBandage) as client:
        global connectedToBandageDevice
        connectedToBandageDevice = True
        await client.start_notify("19b10001-e8f2-537e-4f6c-d104768a1214", handle_rotation_change)
        await client.start_notify("19B10002-E8F2-537E-4F6C-D104768A1214", handle_bandage_reset_change)
        print("connected to Bandage Device")
        bandage_connected = True
        # Continuously run the loop while True:
        while connectedToBandageDevice:
            await asyncio.sleep(0.001)

connectedToPillDevice = False
pillClient = None

async def connectToPillDevice():
    def disconnectedFromPill(disconnectArg):
        global connectedToPillDevice
        print("disconnected from Pill Device")
        connectedToPillDevice = False

    async with BleakClient("34:94:54:27:F7:8A", disconnected_callback=disconnectedFromPill) as client:
        print("connected to Pill Device")
        global connectedToPillDevice
        connectedToPillDevice = True
        global pillClient
        pillClient = client
        await client.start_notify("19b10004-e8f2-537e-4f6c-d104768a1214", handle_Dispensed_change)
        await client.start_notify("19b10005-e8f2-537e-4f6c-d104768a1214", handle_Pill_reset_change)
        while connectedToPillDevice:
            await asyncio.sleep(0.001)




connectedToCreamMeasurer = False

async def connecttoCreamMeasurer():
    def disconnectedFromCream(disconnectArg):
        print("disconnected from Cream Measurer")
        global connectedToCreamMeasurer
        connectedToCreamMeasurer = False
    async with BleakClient("30:C6:F7:02:FD:52", disconnected_callback=disconnectedFromCream) as client:
        print("connected to Cream Measurer")
        global connectedToCreamDevice
        connectedToCreamDevice = True
        await client.start_notify("19b10007-e8f2-537e-4f6c-d104768a1214", handle_Distance_change)
        await client.start_notify("19b10008-e8f2-537e-4f6c-d104768a1214", handle_cream_reset_change)
        Cream_Measurer = True
        while True:
            await asyncio.sleep(0.001)

refreshRequested = False

def refreshHome(home):
    print("Refreshing home...")
    time.sleep(0.1)
    home.refresh()
    refreshRequested = False

def refreshRequest(home):
    if refreshRequested:
        return
    
    refreshRequested = True
    t = Thread(target=refreshHome, args=[home])
    t.start()

#class Supply:
#    def __init__(self, id, name, category, quantity, refillSize, date):
#        self.id = id
#        self.name = name
#        self.category = category
#        self.quantity = quantity
#        self.refillSize = refillSize
#        self.data = date
#
#    def __str__(self):
#        return (str(self.id) + self.name + self.category + str(self.quantity) + str(self.refillSize) + self.date)

class Login(ctk.CTkToplevel):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.title("Login System")
        self.minsize(500, 350)

        loginFrame = ctk.CTkFrame(master=self)
        loginFrame.pack(pady=20, padx=60, fill="both", expand=True)

        label = ctk.CTkLabel(master=loginFrame, text="Login System", font=("Roboto-Black", 32))
        label.pack(pady=12, padx=10)

        entry = ctk.CTkEntry(master=loginFrame, placeholder_text="User ID")
        entry.pack(pady=12, padx=10)
        self.entry = entry

        loginButton = ctk.CTkButton(master=loginFrame, text="Log In", command=self.checkUser)
        loginButton.pack(pady=12, padx=10)
    
        newUserButton = ctk.CTkButton(master=loginFrame, text="New User", command=self.newUser)
        newUserButton.pack(pady=12, padx=10)

    def newUser(self):
        if os.path.isfile("user_data.json") is False:
            user_data = {}
        else:
            fd = open("user_data.json", "r")
            txt = fd.read()
            user_data = json.loads(txt)
            fd.close()

        global user_id

        if len(user_data.keys()) == 0:
            user_id = 1000
        else:
            user_id = int(list(user_data.keys())[-1]) + 1
        
        user_id = str(user_id)
        time_date = []
        usage_no = []
        name = []
        category = []
        quantity_all = []
        prod_id = []
        transaction_id = "".join(
            random.choice("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ") for i in range(10)
        )

        print("Welcome New User!")
        print("Your user ID is " + user_id)

        if user_id not in user_data.keys():
            user_data[user_id] = {}

        global isLoggedIn
        isLoggedIn = True

        self.withdraw()
        global home
        home.updateUserLabel()
        home.deiconify()

        loginButton = home.getLoginButton()
        loginButton.configure(text="Sign Out")
        loginButton.configure(command=home.signOut)

        js = json.dumps(user_data)
        fd = open("user_data.json", "w")
        fd.write(js)
        fd.close()

    def checkUser(self):
        if os.path.isfile("user_data.json") is False:
            user_data = {}
        else:
            fd = open("user_data.json", "r")
            txt = fd.read()
            user_data = json.loads(txt)
            fd.close()

        global user_id
        userInput = self.entry.get()
        global isLoggedIn

        if (userInput in user_data.keys()):
            user_id = userInput
            isLoggedIn = True
            print("Welcome User " + user_id + "!")
            self.withdraw()
            global home
            home.updateUserLabel()
            home.deiconify()

            loginButton = home.getLoginButton()
            loginButton.configure(text="Sign Out")
            loginButton.configure(command=home.signOut)

        else:
            print("Please enter a valid user ID or press \"New User\"")

        js = json.dumps(user_data)
        fd = open("user_data.json", "w")
        fd.write(js)
        fd.close()


class RecordUse(ctk.CTkToplevel):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.title("Record Use")
        self.minsize(550, 375)

        #self.grid_rowconfigure((0, 1, 2, 3), weight=1)
        #self.grid_columnconfigure((0,1,2,3,4,5,6,7,8,9,10,11), weight=1)

        recordUseFrame = ctk.CTkFrame(master=self)
        recordUseFrame.pack(pady=20, padx=60, fill="both", expand=True)

        label = ctk.CTkLabel(master=recordUseFrame, text="Record Use", font=("Roboto-Black", 26))
        label.pack(pady=(32,20), padx=15)

        nameidEntry = ctk.CTkEntry(master=recordUseFrame, placeholder_text="Supply Name or ID")
        nameidEntry.pack(pady=12, padx=10)
        self.nameidEntry = nameidEntry

        quantAddEntry = ctk.CTkEntry(master=recordUseFrame, placeholder_text="Quantity Added")
        quantAddEntry.pack(pady=12, padx=10)
        self.quantAddEntry = quantAddEntry

        quantRemEntry = ctk.CTkEntry(master=recordUseFrame, placeholder_text="Quantity Removed")
        quantRemEntry.pack(pady=12, padx=10)
        self.quantRemEntry = quantRemEntry

        dateEntry = ctk.CTkEntry(master=recordUseFrame, placeholder_text="Date of Usage")
        dateEntry.pack(pady=12, padx=10)
        self.dateEntry = dateEntry

        confirmButton = ctk.CTkButton(master=recordUseFrame, text="Confirm", command=self.confirm)
        confirmButton.pack(pady=12, padx=10)


    def confirm(self):
        tempNameid = self.nameidEntry.get() #tempNameid will be a string, even for IDs
        tempQuantAdd = self.quantAddEntry.get() #tempQuantAdd will be a string
        tempQuantRem = self.quantRemEntry.get() #tempQuantRem will be a string
        date = self.dateEntry.get()
        self.date = date

        # Checking if entries exist and are the correct variable types  
        if tempNameid!=(""): # Checking nameid
            fd = open("data.json", "r")
            txt = fd.read()
            data = json.loads(txt)
            fd.close()

            try:
                tempNameid = int(tempNameid)
            except:
                print("name")
                name = tempNameid
                self.name = name

                found = False
                for i in data.keys():
                    if data[str(i)]["name"] == name:
                        id = i
                        self.id = id
                        found = True
                if (not found):
                    print("Error: Supply name or ID not recognized")
                    self.cancel()
            else:
                print("id")
                id = tempNameid
                self.id = id

            if tempQuantAdd!=(""): # Checking quantAdd
                try:
                    quantAdd = int(tempQuantAdd)
                except:
                    print("Error: Quantity Added should be an int")
                    self.cancel()
                else:
                    quantAdd = int(tempQuantAdd)
                    self.quantAdd = quantAdd
            else:
                quantAdd = 0
                self.quantAdd = quantAdd

            if tempQuantRem!=(""): # Checking quantRem
                try:
                    quantRem = int(tempQuantRem)
                except:
                    print("Error: Quantity Removed should be an int")
                    self.cancel()
                else:
                    quantRem = int(tempQuantRem)
                    self.quantRem = quantRem
            else:
                quantRem = 0
                self.quantRem = quantRem

            self.recordUse()
        else:
            print("Error: please enter a supply name or id")
            self.cancel()

    def recordUse(self):
        print("recordUse")

        addToData(self.id, "quantity", self.quantAdd-self.quantRem)
        replaceData(self.id, "date", self.date)

        self.withdraw()
        global home
        home.deiconify()
        home.refresh()

    def cancel(self):
        print("cancel")
        self.withdraw()
        global home
        home.deiconify()
        home.refresh()


class AddItem(ctk.CTkToplevel):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.title("Add Item")
        self.minsize(550, 375)

        #self.grid_rowconfigure((0, 1, 2, 3), weight=1)
        #self.grid_columnconfigure((0,1,2,3,4,5,6,7,8,9,10,11), weight=1)

        addItemFrame = ctk.CTkFrame(master=self)
        addItemFrame.pack(pady=20, padx=60, fill="both", expand=True)

        label = ctk.CTkLabel(master=addItemFrame, text="Add New Item to Database", font=("Roboto-Black", 26))
        label.pack(pady=(32,20), padx=15)

        nameEntry = ctk.CTkEntry(master=addItemFrame, placeholder_text="Name")
        nameEntry.pack(pady=12, padx=10)
        self.nameEntry = nameEntry

        quantityEntry = ctk.CTkEntry(master=addItemFrame, placeholder_text="Quantity")
        quantityEntry.pack(pady=12, padx=10)
        self.quantityEntry = quantityEntry

        unitEntry = ctk.CTkEntry(master=addItemFrame, placeholder_text="Unit")
        unitEntry.pack(pady=12, padx=10)
        self.unitEntry = unitEntry

        # Change category from entry to dropdown for final product.
        categoryEntry = ctk.CTkEntry(master=addItemFrame, placeholder_text="Category")
        categoryEntry.pack(pady=12, padx=10)
        self.categoryEntry = categoryEntry

        dateEntry = ctk.CTkEntry(master=addItemFrame, placeholder_text="Date Added")
        dateEntry.pack(pady=12, padx=10)
        self.dateEntry = dateEntry

        confirmButton = ctk.CTkButton(master=addItemFrame, text="Confirm", command=self.confirm)
        confirmButton.pack(pady=12, padx=10)


    def confirm(self):
        global home
        name = self.nameEntry.get()
        self.name = name
        tempQuantity = self.quantityEntry.get() #tempQuantity will be a string
        unit = self.unitEntry.get()
        self.unit = unit
        category = self.categoryEntry.get()
        self.category = category
        date = self.dateEntry.get()
        self.date = date

        # Checking if entries exist and are the correct variable types    
        if isinstance(name, str) and name!=(""):
            if tempQuantity!=(""):
                try:
                    quantity = int(tempQuantity)
                except:
                    print("Error: Quantity must be an int.")
                else:
                    quantity = int(tempQuantity)
                    self.quantity = quantity
                    print("Confirmed")
                    home.addItem()
            else:
                print("Error: Please enter a value for quantity.")
        elif not isinstance(name, str):
            print("Error: Name must be a string.")
        elif name==(""):
            print("Error: Please enter a value for name.")


    def getName(self):
        return self.name
    def getQuantity(self):
        return self.quantity
    def getUnit(self):
        return self.unit
    def getCategory(self):
        return self.category
    def getDate(self):
        return self.date


class RemoveItem(ctk.CTkToplevel):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.title("Remove Item")
        self.minsize(550, 375)

        removeItemFrame = ctk.CTkFrame(master=self)
        removeItemFrame.pack(pady=20, padx=60, fill="both", expand=True)

        label = ctk.CTkLabel(master=removeItemFrame, text="Type Item ID to Confirm Removal", font=("Roboto-Black", 26))
        label.pack(pady=(32,20), padx=15)

        entry = ctk.CTkEntry(master=removeItemFrame, placeholder_text="Item ID")
        entry.pack(pady=12, padx=15)
        self.entry = entry

        confirmButton = ctk.CTkButton(master=removeItemFrame, text="Confirm", command=self.confirm)
        confirmButton.pack(pady=12, padx=10)

        cancelButton = ctk.CTkButton(master=removeItemFrame, text="Cancel", command=self.cancel)
        cancelButton.pack(pady=12, padx=10)

    def confirm(self):
        fd = open("data.json", "r")
        txt = fd.read()
        data = json.loads(txt)
        fd.close()

        id = self.entry.get()
        if id in data.keys():
            global home
            data.pop(id)
            home.deleteDataRow(id)

            self.withdraw()
            home.deiconify()
            home.refresh()
        else:
            print("Invalid Supply ID")

        js = json.dumps(data)
        fd = open("data.json", "w")
        fd.write(js)
        fd.close()

    def cancel(self):
        self.withdraw()
        global home
        home.deiconify()    


class Home(ctk.CTk):
    def __init__(self):
        super().__init__()

        fd = open("data.json", "r")
        txt = fd.read()
        data = json.loads(txt)
        fd.close()

        main_font = ctk.CTkFont(family="Microsoft Sans Serif Regular", size=15)
        self.main_font = main_font

        self.geometry("900x550")
        self.title("Censeo Inventory")

        self.grid_rowconfigure((0, 1, 2, 3), weight=1)
        self.grid_columnconfigure((0,1,2,3,4,5,6), weight=1)

        frame1 = ctk.CTkFrame(master=self)
        frame1.pack(pady=(20,0), padx=60, fill="both", expand=True)

        scrollFrame = ctk.CTkScrollableFrame(master=self)
        scrollFrame.pack(pady=10, padx=60, fill="both", expand=True)
        #scrollFrame.configure( *SOMETHING TO ELIMINATE CORNERS* )
        scrollFrame.grid_rowconfigure((0, 1, 2, 3), weight=1)
        self.scrollFrame = scrollFrame

        frame2 = ctk.CTkFrame(master=self)
        frame2.pack(pady=(0,20), padx=60, fill="both", expand=True)

        # CREATING FRAME1 WIDGETS

        userLabel = ctk.CTkLabel(master=frame1, font=(main_font, 18))
        #printButton.pack(pady=12, padx=10)
        userLabel.grid(row=0, column=0, columnspan=2, padx=(40,0), pady=(50,30), sticky="nsew")
        self.userLabel = userLabel
        self.updateUserLabel()

        homeLabel = ctk.CTkLabel(master=frame1, text="Censeo Inventory", font=(main_font,30))
        #homeLabel.pack(pady=12, padx=10)
        homeLabel.grid(row=0, column=2, columnspan=3, padx=(0,15), pady=(50,30), sticky="nsew")
        self.homeLabel = homeLabel

        loginButton = ctk.CTkButton(master=frame1, text="Log In", command=self.enterLogin, font=(main_font, 14), fg_color="#0b5394")
        #loginButton.pack(pady=12, padx=10)
        loginButton.grid(row=0, column=5, columnspan=2, padx=(25,50), pady=(50,30), sticky="nsew")
        self.loginButton = loginButton

        #Title Text Boxes

        idTitle = ctk.CTkTextbox(master=frame1, width=100, height=20, font=(main_font, 14), border_color="#0b5394", border_width=3)
        idTitle.grid(row=2, column=0, columnspan=2, padx=(115,5), pady=(5,0), sticky="nsew")
        self.idTitle = idTitle
        self.idTitle.insert("0.0", "Supply ID")
        idTitle.configure(state="disabled")

        nameTitle = ctk.CTkTextbox(master=frame1, width=100, height=20, font=(main_font, 14), border_color="#0b5394", border_width=3)
        nameTitle.grid(row=2, column=2, columnspan=1, padx=5, pady=(5,0), sticky="nsew")
        self.nameTitle = nameTitle
        self.nameTitle.insert("0.0", "Name")
        nameTitle.configure(state="disabled")

        quantityTitle = ctk.CTkTextbox(master=frame1, width=100, height=25, font=(main_font, 14), border_color="#0b5394", border_width=3)
        quantityTitle.grid(row=2, column=3, columnspan=1, padx=5, pady=(5,0), sticky="nsew")
        self.quantityTitle = quantityTitle
        self.quantityTitle.insert("0.0", "Quantity")
        quantityTitle.configure(state="disabled")

        categoryTitle = ctk.CTkTextbox(master=frame1, width=100, height=25, font=(main_font, 14), border_color="#0b5394", border_width=3)
        categoryTitle.grid(row=2, column=4, columnspan=1, padx=5, pady=(5,0), sticky="nsew")
        self.categoryTitle = categoryTitle
        self.categoryTitle.insert("0.0", "Category")
        categoryTitle.configure(state="disabled")

        lastEditedTitle = ctk.CTkTextbox(master=frame1, width=100, height=25, font=(main_font, 14), border_color="#0b5394", border_width=3)
        lastEditedTitle.grid(row=2, column=5, columnspan=1, padx=(5,105), pady=(5,0), sticky="nsew")
        self.lastEditedTitle = lastEditedTitle
        self.lastEditedTitle.insert("0.0", "Last Edited")
        lastEditedTitle.configure(state="disabled")

        #CREATING SCROLLFRAME WIDGETS

        #Data Table

        #Remove Buttons
        self.removeButtons = []
        for i in data.keys():
            removeButton = ctk.CTkButton(master=scrollFrame, text="-", command=self.enterRemoveItem, font=(main_font, 20), fg_color="#0b5394", width=30)
            removeButton.grid(row=i, column=1, padx=(70,5))
            self.removeButtons.append(removeButton)

        #Supply IDs
        self.idEntries = []
        for i in data.keys():
            table = ctk.CTkEntry(scrollFrame, width=100, font=(main_font, 14))
            table.grid(row=i, column=2, padx=5)
            table.insert(ctk.END, str(i))
            table.configure(state="disabled")
            self.idEntries.append(table)

        #Names
        self.nameEntries = []
        for i in data.keys():
            table = ctk.CTkEntry(scrollFrame, width=100, font=(main_font, 14))
            table.grid(row=i, column=3, padx=5)
            supplyID = i
            name = data[supplyID]["name"]
            table.insert(ctk.END, str(name))
            table.configure(state="disabled")
            self.nameEntries.append(table)

        #Quantities
        self.quantityEntries = []
        for i in data.keys():
            table = ctk.CTkEntry(scrollFrame, width=100, font=(main_font, 14))
            table.grid(row=i, column=4, padx=5)
            supplyID = i
            quantity = data[supplyID]["quantity"]
            unit = data[supplyID]["unit"]
            table.insert(ctk.END, str(quantity)+" "+unit)
            table.configure(state="disabled")
            self.quantityEntries.append(table)

        #Categories
        self.categoryEntries = []
        for i in data.keys():
            table = ctk.CTkEntry(scrollFrame, width=100, font=(main_font, 14))
            table.grid(row=i, column=5, padx=(5))
            supplyID = i
            category = data[supplyID]["category"]
            table.insert(ctk.END, str(category))
            table.configure(state="disabled")
            self.categoryEntries.append(table)

        #Last Edited Dates
        self.lastEditedEntries = []
        for i in data.keys():
            table = ctk.CTkEntry(scrollFrame, width=100, font=(main_font, 14))
            table.grid(row=i, column=6, padx=5)
            supplyID = i
            lastEdited = data[supplyID]["date"]
            table.insert(ctk.END, str(lastEdited))
            table.configure(state="disabled")
            self.lastEditedEntries.append(table)
        

        # CREATING WIDGETS FOR FRAME2

        recordUseButton = ctk.CTkButton(master=frame2, text="Record Use", command=self.enterRecordUse, font=(main_font, 14), fg_color="#0b5394")
        recordUseButton.grid(row=0, column=0, columnspan=3, padx=(155,25), pady=(30,15), sticky="nsew")
        self.recordUseButton = recordUseButton

        addItemButton = ctk.CTkButton(master=frame2, text="Add Item", command=self.enterAddItem, font=(main_font, 14), fg_color="#0b5394")
        addItemButton.grid(row=0, column=3, columnspan=3, padx=(0,25), pady=(30,15) , sticky="nsew")
        self.addItemButton = addItemButton

        updateButton = ctk.CTkButton(master=frame2, text="Update", command=self.update, font=(main_font, 14), fg_color="#0b5394")
        updateButton.grid(row=0, column=6, columnspan=3, padx=(0,25), pady=(30,15) , sticky="nsew")
        self.updateButton = updateButton

        #checkButton = ctk.CTkButton(master=frame2, text="Check", command=self.checkBluetooth, font=(main_font, 14), fg_color="#0b5394")
        #checkButton.grid(row=0, column=9, columnspan=3, padx=(0,25), pady=(30,15) , sticky="nsew")
        #self.checkButton = checkButton

        self.login_window = None

        self.addItem_window = None

        self.recordUse_window = None

        self.removeItem_window = None

        js = json.dumps(data)
        fd = open("data.json", "w")
        fd.write(js)
        fd.close()

    def enterLogin(self):
        self.iconify()  # maybe switch to self.withdraw() for presentation
        if self.login_window is None or not self.login_window.winfo_exists():
            self.login_window = Login(self)  # create window if its None or destroyed
        else:
            self.login_window.deiconify()
            self.login_window.focus()  # if window exists focus it

    def signOut(self):
        global user_id
        user_id = 0
        global isLoggedIn
        isLoggedIn = False
        print("You have signed out.")
        self.updateUserLabel()

        self.loginButton.configure(text="Log In")
        self.loginButton.configure(command=self.enterLogin)

    def print(self):
        global isLoggedIn
        print(isLoggedIn)

    def getLoginButton(self):
        return self.loginButton
    
    def updateUserLabel(self):
        global user_id
        if(user_id == 0):
            self.userLabel.configure(text="Signed Out")
        else:
            self.userLabel.configure(text="User " + str(user_id))

    def enterRecordUse(self):
        print("Record Use")
        global isLoggedIn
        if isLoggedIn:
            self.iconify()  # maybe switch to self.withdraw() for presentation
            if self.recordUse_window is None or not self.recordUse_window.winfo_exists():
                self.recordUse_window = RecordUse(self)  # create window if its None or destroyed
            else:
                self.recordUse_window.deiconify()
                self.recordUse_window.focus()  # if window exists focus it
        else:
            print("Please log in before recording use")

    def enterAddItem(self):
        global isLoggedIn
        if isLoggedIn:
            self.iconify()  # maybe switch to self.withdraw() for presentation
            if self.addItem_window is None or not self.addItem_window.winfo_exists():
                self.addItem_window = AddItem(self)  # create window if its None or destroyed
            else:
                self.addItem_window.deiconify()
                self.addItem_window.focus()  # if window exists focus it
        else:
            print("Please log in before adding item")

    def enterRemoveItem(self):
        global isLoggedIn
        if isLoggedIn:
            self.iconify()
            if self.removeItem_window is None or not self.removeItem_window.winfo_exists():
                self.removeItem_window = RemoveItem(self)  # create window if its None or destroyed
            else:
                self.removeItem_window.deiconify()
                self.removeItem_window.focus()  # if window exists focus it
        else:
            print("Please log in before removing item")

    def refresh(self):
        print("Refresh")

        fd = open("data.json", "r")
        txt = fd.read()
        data = json.loads(txt)
        fd.close()

        scrollFrame = self.scrollFrame
        main_font = self.main_font

        #Supply IDs
        for i in range(len(self.idEntries)):
            idEntry = self.idEntries[i]
            idEntry.configure(state="normal")
            idEntry.delete(0, len(idEntry.get()))
            idEntry.insert(ctk.END, readID(i))
            idEntry.configure(state="disabled")
            
        #Names
        for i in range(len(self.nameEntries)):
            nameEntry = self.nameEntries[i]
            nameEntry.configure(state="normal")
            nameEntry.delete(0, len(nameEntry.get()))
            for j in data.keys():
                if (str(i+1)==j):
                    id = j
            nameEntry.insert(ctk.END, data[id]["name"])
            nameEntry.configure(state="disabled")

        #Quantities
        for i in range(len(self.quantityEntries)):
            quantityEntry = self.quantityEntries[i]
            quantityEntry.configure(state="normal")
            quantityEntry.delete(0, len(quantityEntry.get()))
            for j in data.keys():
                if (str(i+1)==j):
                    id = j
            quantityEntry.insert(ctk.END, str(data[id]["quantity"])+" "+data[id]["unit"])
            quantityEntry.configure(state="disabled")

        #Categories
        for i in range(len(self.categoryEntries)):
            categoryEntry = self.categoryEntries[i]
            categoryEntry.configure(state="normal")
            categoryEntry.delete(0, len(categoryEntry.get()))
            for j in data.keys():
                if (str(i+1)==j):
                    id = j
            categoryEntry.insert(ctk.END, data[id]["category"])
            categoryEntry.configure(state="disabled")

        #Last Edited Dates
        for i in range(len(self.lastEditedEntries)):
            lastEditedEntry = self.lastEditedEntries[i]
            lastEditedEntry.configure(state="normal")
            lastEditedEntry.delete(0, len(lastEditedEntry.get()))
            for j in data.keys():
                if (str(i+1)==j):
                    id = j
            lastEditedEntry.insert(ctk.END, data[id]["date"])
            lastEditedEntry.configure(state="disabled")

        js = json.dumps(data)
        fd = open("data.json", "w")
        fd.write(js)
        fd.close()

    def update(self):
        self.refresh()

    def addItem(self):
        print("addItem")

        fd = open("data.json", "r")
        txt = fd.read()
        data = json.loads(txt)
        fd.close()

        global addItem
        scrollFrame = self.scrollFrame
        main_font = self.main_font

        data[str(len(data.keys())+1)] = {
            "name": self.addItem_window.getName(),
            "quantity": self.addItem_window.getQuantity(),
            "unit": self.addItem_window.getUnit(),
            "category": self.addItem_window.getCategory(),
            "date": self.addItem_window.getDate(),
        }

        r = len(data.keys())
        id = str(r)

        js = json.dumps(data)
        fd = open("data.json", "w")
        fd.write(js)
        fd.close()
        fd = open("data.json", "r")
        txt = fd.read()
        data = json.loads(txt)
        fd.close()

        removeButton = ctk.CTkButton(master=scrollFrame, text="-", command=self.enterRemoveItem(), font=(main_font, 20), fg_color="#0b5394", width=30)
        removeButton.grid(row=r, column=1, padx=(70,5))

        idTable = ctk.CTkEntry(self.scrollFrame, width=100, font=(self.main_font, 14))
        idTable.grid(row=r, column=2, padx=5)
        idTable.insert(ctk.END, id)
        idTable.configure(state="disabled")
        self.idEntries.append(idTable)

        nameTable = ctk.CTkEntry(scrollFrame, width=100, font=(main_font, 14))
        nameTable.grid(row=r, column=3, padx=5)
        supplyID = id
        name = data[supplyID]["name"]
        nameTable.insert(ctk.END, str(name))
        nameTable.configure(state="disabled")
        self.nameEntries.append(nameTable)

        quantityTable = ctk.CTkEntry(scrollFrame, width=100, font=(main_font, 14))
        quantityTable.grid(row=r, column=4, padx=5)
        supplyID = id
        quantity = data[supplyID]["quantity"]
        unit = data[supplyID]["unit"]
        quantityTable.insert(ctk.END, str(quantity)+" "+unit)
        quantityTable.configure(state="disabled")
        self.quantityEntries.append(quantityTable)

        categoryTable = ctk.CTkEntry(scrollFrame, width=100, font=(main_font, 14))
        categoryTable.grid(row=r, column=5, padx=(5))
        supplyID = id
        category = data[supplyID]["category"]
        categoryTable.insert(ctk.END, str(category))
        categoryTable.configure(state="disabled")
        self.categoryEntries.append(categoryTable)

        lastEditedTable = ctk.CTkEntry(scrollFrame, width=100, font=(main_font, 14))
        lastEditedTable.grid(row=r, column=6, padx=5)
        supplyID = id
        lastEdited = data[supplyID]["date"]
        lastEditedTable.insert(ctk.END, str(lastEdited))
        lastEditedTable.configure(state="disabled")
        self.lastEditedEntries.append(lastEditedTable)

        self.addItem_window.withdraw()
        self.deiconify()

        js = json.dumps(data)
        fd = open("data.json", "w")
        fd.write(js)
        fd.close()

        self.refresh()

    def deleteDataRow(self, i):
        self.removeButtons[int(i)-1].destroy()
        del self.removeButtons[int(i)-1]
        self.idEntries[int(i)-1].destroy()
        del self.idEntries[int(i)-1]
        self.nameEntries[int(i)-1].destroy()
        del self.nameEntries[int(i)-1]
        self.categoryEntries[int(i)-1].destroy()
        del self.categoryEntries[int(i)-1]
        self.quantityEntries[int(i)-1].destroy()
        del self.quantityEntries[int(i)-1]
        self.lastEditedEntries[int(i)-1].destroy()
        del self.lastEditedEntries[int(i)-1]


def replaceData(itemID, itemDataType, newData): #replaces a data entry in data.json with a new value
    #third parameter should be the same variable type as the existing entry in data.json
    
    fd = open("data.json", "r")
    txt = fd.read()
    data = json.loads(txt)
    fd.close()

    data[str(itemID)][str(itemDataType)] = newData

    js = json.dumps(data)
    fd = open("data.json", "w")
    fd.write(js)
    fd.close()


def addToData(itemID, itemDataType, deltaData): #adds deltaData to the existing data entry
    #Only for data entries that are integers (like quantity)

    fd = open("data.json", "r")
    txt = fd.read()
    data = json.loads(txt)
    fd.close()

    data[str(itemID)][str(itemDataType)] += deltaData

    js = json.dumps(data)
    fd = open("data.json", "w")
    fd.write(js)
    fd.close()


def readData(itemID, itemDataType): #returns the data entry under itemID and itemDataType
    fd = open("data.json", "r")
    txt = fd.read()
    data = json.loads(txt)
    fd.close()

    dataRead = data[str(itemID)][str(itemDataType)]

    js = json.dumps(data)
    fd = open("data.json", "w")
    fd.write(js)
    fd.close()

    return dataRead


def readID(i): #returns the id of the ith item
    fd = open("data.json", "r")
    txt = fd.read()
    data = json.loads(txt)
    fd.close()

    keys = list(data.keys())

    js = json.dumps(data)
    fd = open("data.json", "w")
    fd.write(js)
    fd.close()

    return keys[i]


def handle_rotation_change(sender, data):
    rotation = struct.unpack('<L', data)
    print(rotation[0])
    if (rotation[0]<4294967295/2):
        deltaLength = 360 - calcBandage(rotation[0])
    else:
        deltaLength = 360 - calcBandage(4294967294-4294967295)
    if (deltaLength > 0):
        replaceData("3", "quantity", deltaLength)
    else:
        replaceData("3", "quantity", 0)
    
    home.refresh()

def handle_bandage_reset_change(sender, data):
    print(data)
    reset = struct.unpack('<b', data)
    print(reset[0])
    replaceData("3", "quantity", 360)

    home.refresh()

async def handle_Dispensed_change(sender, data):
    print(data)
    Dispensed = struct.unpack('<b', data)
    print(Dispensed[0])
    currentQuantity = int(readData("2", "quantity"))
    print("currentQuantity=%s" % currentQuantity)
    currentQuantity = currentQuantity - 1
    global pillClient
    await pillClient.write_gatt_char("19b10006-e8f2-537e-4f6c-d104768a1214", currentQuantity.to_bytes(1, 'little'))
    if (currentQuantity > 0):
        replaceData("2", "quantity", str(currentQuantity))
    else:
        replaceData("2", "quantity", 0)
    #Write new quantity to peripheral characteristic
    home.refresh()

def handle_Pill_reset_change(sender, data):
    print(data)
    Pill_reset = struct.unpack('<b', data)
    print(Pill_reset[0])
    replaceData("2", "quantity", 50)
    home.refresh()

def handle_Distance_change(sender, data):
    print(data)
    Moved = struct.unpack('<L', data)
    print(Moved)
    if(calcCream(Moved[0])<readData("1", "quantity")):
        replaceData("1", "quantity", calcCream(Moved[0]))
        home.refresh()

def handle_cream_reset_change(sender, data):
    print(data)
    Cream_reset = struct.unpack('<b', data)
    print(Cream_reset[0])
    replaceData("1", "quantity", 132)
    home.refresh()
    


def bandageThread():
    while True:
        try:
            print("trying to connect to Bandage Device...")
            asyncio.run(connecttoBandageDevice())
        except Exception as error: 
            print("Bandage device error: " + str(error))

def pillThread():
    while True:
        try:
            print("trying to connect to Pill Device...")
            asyncio.run(connectToPillDevice())
        except Exception as error: 
            print("Pill device error: " + str(error))

def creamThread():
    while True:
        try:
            print("trying to connect to Cream Device...")
            asyncio.run(connecttoCreamMeasurer())
        except Exception as error: 
            print("Cream device error: " + str(error))

def calcBandage(increments):
    diameter = 0.945 #in
    pi = 3.141
    incrementsPerRotation = 30
    incrementLength = diameter*pi/incrementsPerRotation
    deltaLength = incrementLength*increments
    return int(deltaLength)

def calcCream(increments):
        totalIncrements = 12
        incrementLength = int(175/totalIncrements)

        if (increments<=totalIncrements):
            r = 18
            w = 54
            l1 = 175-increments*incrementLength
            l2 = 0.001
            #total length = 175

            upperz1 = l1
            uppery1 = lambda z: (-r)*z/l1+r
            upperx1 = lambda z, y: (w/2-r)*z/l1+r

            upperz2 = l2
            uppery2 = lambda z: (-r)*z/l2+r
            upperx2 = lambda z, y: (w/2-r)*z/l2+r

            f = lambda z, y, x: 1

            solve1 = integrate.tplquad(f, 0, upperz1, 0, uppery1, 0, upperx1)
            solve2 = integrate.tplquad(f, 0, upperz2, 0, uppery2, 0, upperx2)
            solution = 4*(solve1[0]-solve2[0])/1000
            error = solve1[1]+solve2[1]
            print(int(solution))
            return(int(solution))
        else:
            print("Error: increments out of range")
            return readData("1", "quantity")

if __name__ == "__main__":
    t1 = Thread(target = bandageThread)
    #t1.start()

    t2 = Thread(target = pillThread)
    t2.start()
    
    t3 = Thread(target = creamThread)
    #t3.start()
    
    home = Home()
    home.mainloop()
