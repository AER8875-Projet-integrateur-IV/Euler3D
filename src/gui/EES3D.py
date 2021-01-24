
from tkinter import *

from random import randrange

# creer la fenetre mere
root = Tk()


def form1():

    fen.delete("all")

    #fen.geometry('700x900')

    # Creation de zone de texte pour formulaire
    textquestion = Label(fen, text='Mesh \n SU2(Format)')
    textquestion.grid(row=0, column=0,)

    # Definition du formulaire
    textform = Entry(fen, width=30)
    textform.grid(row=0, column=2)

    # Zone de text affichae pour quitter

    # creation de bouton
    MenuButton1 = Button(fen, text='Run Solver', command='')
    MenuButton1.grid(row=8, column=1, )
    MenuButton2 = Button(fen, text='Generate Controle File', command='')
    MenuButton2.grid(row=8, column=2, )

def form2():

        fen.delete("all")

        #fen.geometry('700x900')

        # Creation de zone de texte pour formulaire
        textquestion = Label(fen, text='Speed Type')
        textquestion.grid(row=0, column=0, )

        textquestion1 = Label(fen, text='Speed Value')
        textquestion1.grid(row=1, column=0, )

        textquestion2 = Label(fen, text='Angle Of Attaque')
        textquestion2.grid(row=2, column=0, )

        textquestion3 = Label(fen, text='Pressure')
        textquestion3.grid(row=3, column=0, )

        textquestion4 = Label(fen, text='Temperature')
        textquestion4.grid(row=4, column=0, )

        textquestion5 = Label(fen, text='Viscosity')
        textquestion5.grid(row=5, column=0, )

        textquestion6 = Label(fen, text='Density')
        textquestion6.grid(row=6, column=0, )

        textquestion7 = Label(fen, text='Gamma')
        textquestion7.grid(row=7, column=0, )

        textquestion8 = Label(fen, text='Gas Constat')
        textquestion8.grid(row=8, column=0, )

        textquestion9 = Label(fen, text='Specific Helf')
        textquestion9.grid(row=9, column=0, )

        # Definition du formulaire
        textform = Entry(fen, width=30)
        textform.grid(row=0, column=2, pady=10)

        textform1 = Entry(fen, width=30)
        textform1.grid(row=1, column=2, pady=10)

        textform2 = Entry(fen, width=30)
        textform2.grid(row=2, column=2, pady=10)

        textform3 = Entry(fen, width=30)
        textform3.grid(row=3, column=2, pady=10)

        textform4 = Entry(fen, width=30)
        textform4.grid(row=4, column=2, pady=10)

        textform5 = Entry(fen, width=30)
        textform5.grid(row=5, column=2, pady=10)

        textform6 = Entry(fen, width=30)
        textform6.grid(row=6, column=2, pady=10)

        textform7 = Entry(fen, width=30)
        textform7.grid(row=7, column=2, pady=10)

        textform8 = Entry(fen, width=30)
        textform8.grid(row=6, column=2, pady=10)

        textform9 = Entry(fen, width=30)
        textform9.grid(row=6, column=2, pady=10)

        # creation de bouton
        MenuButton1 = Button(fen, text='Run Solver', command='')
        MenuButton1.grid(row=8, column=1, )
        MenuButton2 = Button(fen, text='Generate Controle File', command='')
        MenuButton2.grid(row=8, column=2, )


def form3():

    fen.delete("all")

    #fen.geometry('700x900')

    # Creation de zone de texte pour formulaire
    textquestion = Label(fen, text='Time Integration')
    textquestion.grid(row=0, column=0, )

    textquestion1 = Label(fen, text='CFL')
    textquestion1.grid(row=1, column=0, )

    textquestion2 = Label(fen, text='Maximal Residu')
    textquestion2.grid(row=2, column=0, )

    textquestion3 = Label(fen, text='Max Interaction Number')
    textquestion3.grid(row=3, column=0, )

    textquestion4 = Label(fen, text='Number Of Threa')
    textquestion4.grid(row=4, column=0, )

    # Definition du formulaire

    textform = Entry(fen, width=30)
    textform.grid(row=0, column=2, pady=10)

    textform1 = Entry(fen, width=30)
    textform1.grid(row=1, column=2, pady=10)

    textform2 = Entry(fen, width=30)
    textform2.grid(row=2, column=2, pady=10)

    textform3 = Entry(fen, width=30)
    textform3.grid(row=3, column=2, pady=10)

    textform4 = Entry(fen, width=30)
    textform4.grid(row=4, column=2, pady=10)

    # creation de bouton
    MenuButton1 = Button(fen, text='Run Solver', command='')
    MenuButton1.grid(row=8, column=1, )
    MenuButton2 = Button(fen, text='Generate Controle File', command='')
    MenuButton2.grid(row=8, column=2, )


def form4():

    fen.delete("all")

    #fen.geometry('700x900')

    # Creation de zone de texte pour formulaire
    textquestion = Label(fen, text='Pressure File')
    textquestion.grid(row=0, column=0, )

    textquestion1 = Label(fen, text='Residual File')
    textquestion1.grid(row=1, column=0, )

    textquestion2 = Label(fen, text='Out Put VTK File')
    textquestion2.grid(row=2, column=0, )

    # Definition du formulaire

    textform = Entry(fen, width=30)
    textform.grid(row=0, column=2, pady=10)

    textform1 = Entry(fen, width=30)
    textform1.grid(row=1, column=2, pady=10)

    textform2 = Entry(fen, width=30)
    textform2.grid(row=2, column=2, pady=10)

    # creation de bouton
    MenuButton1 = Button(fen, text='Run Solver', command='')
    MenuButton1.grid(row=8, column=1, )
    MenuButton2 = Button(fen, text='Generate Controle File', command='')
    MenuButton2.grid(row=8, column=2, )



SIDE = 500


fen = Canvas(root, width=500, height=600, background="green")
fen.pack(side=BOTTOM)

bouton = Button(root, text="PRE-PROCESSING", command=form1)
bouton1 = Button(root, text="SIMULATION", command=form2)
bouton2 = Button(root, text="SOLVER", command=form3)
bouton3 = Button(root, text="POST-PROCESSING", command=form4)

bouton.pack(side=LEFT)
bouton1.pack(side=LEFT)
bouton2.pack(side=LEFT)
bouton3.pack(side=LEFT)
fen.mainloop()