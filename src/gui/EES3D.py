import os
import tkinter as tk
from datetime import datetime
from tkinter import ttk, filedialog
from tkinter.filedialog import askopenfilename
import tkinter as TK

import self


class SeaofBTCapp(tk.Tk):

    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)
        container = tk.Frame(self)

        container.pack(side="top", fill="both", expand=True)

        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        self.frames = {}

        for F in (StartPage, Preprocessing, Solver, Simulation, Postprocessing):
            frame = F(container, self)

            self.frames[F] = frame

            frame.grid(row=0, column=0, sticky="nsew")

        self.show_frame(StartPage)

    def show_frame(self, cont):
        frame = self.frames[cont]
        frame.tkraise()


# ----------------------------------------------------------
def OpenFile1():
        name1 = askopenfilename(initialdir="/",
                                filetypes=(("Text File", "*.txt"), ("All Files", "*.*")),
                                title="Choose a file."
                                )
        path = os.getcwd()
        print(name1 + path)
        # Using try in case user types in unknown file or closes without choosing a file.
        try:
            with open(name1, 'r') as UseFile:
                print(name1)
        except:
            print("No file exists")


# ----------------------------------------------------------
def ReadFile1():
    name1 = askopenfilename(initialdir="/",
                            filetypes=(("Text File", "*.pa"), ("All Files", "*.*")),
                            title="Choose a file."
                            )
    # Using try in case user types in unknown file or closes without choosing a file.
    try:
        with open(name1, 'x') as UseFile:
            print(name1)
    except:
        print("No file exists")
# ----------------------------------------------------------

class HoverButton(tk.Button):
    def __init__(self, master, **kw):
        tk.Button.__init__(self, master=master, **kw)
        self.defaultBackground = self["background"]
        self.bind("<Enter>", self.on_enter)
        self.bind("<Leave>", self.on_leave)

    def on_enter(self, e):
        self['background'] = self['activebackground']

    def on_leave(self, e):
        self['background'] = self.defaultBackground


# ----------------------------------------------------------

class StartPage(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="BIENVENU DANS L'APPLICATION DE GESTION DE FLUX MECANIQUE")
        label.pack(side="bottom", pady=320, padx=100)

        button1 = tk.Button(self, text="PRE-PROCESSING",
                            command=lambda: controller.show_frame(Preprocessing))
        button1.pack(side="left", padx=5, pady=5, ipadx=30)

        button2 = tk.Button(self, text="SOLVER",
                            command=lambda: controller.show_frame(Solver))
        button2.pack(side="left", padx=5, pady=5, ipadx=30)

        button3 = tk.Button(self, text="SIMULATION",
                            command=lambda: controller.show_frame(Simulation))
        button3.pack(side="left", padx=5, pady=5, ipadx=30)

        button4 = tk.Button(self, text="POST-PRECESSING",
                            command=lambda: controller.show_frame(Postprocessing))
        button4.pack(side="left", padx=5, pady=5, ipadx=30)


class Preprocessing(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="")
        label.pack(side="bottom", pady=320, padx=100)
        label1 = tk.Label(self, font=("Arial", 10),
                          text="MESH FILE \n (SU2 FORMAT):")
        label1.place(x=20, y=220)
        self.file_path = tk.StringVar()
        self.entry1 = tk.Entry(self, textvariable=self.file_path, width="50")
        self.entry1.place(x=170, y=220)
        open_button = HoverButton(self, font=("Arial", 10), text="...", command=OpenFile1, underline=0)
        open_button.place(x=500, y=220)

        def save_info():
            entry_info = self.file_path.get()
            print(entry_info)
            file = open("fichier_test.txt", "w")
            file.write("-------------------------------------------------------------------------------\n"
                       "%%%%%%%%%%%%%%%%     EES33D Software Input file    %%%%%%%%%%%%%%%%%%%%% \n"
                       "Author : PROJET ALI EES3D GUI\n" \
                       "CARTE D'ENTREE GESTIONNAIRE DE FLUX MECANIQUE : \n" \
                       f"Date : {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n"
                       "-------------------------------------------------------------------------------\n"
                       "\nSTART\n\n"
                       "------------------- PRE-PROCESSING CONTROL -------------------\n\n"
                       "MESH FILE \n (SU2 FORMAT): " + entry_info)
            file.write("\n")
            file.close()

        button1 = tk.Button(self, text="PRE-PROCESSING",
                            command=lambda: controller.show_frame(StartPage))
        button1.pack(side="left", padx=5, pady=5, ipadx=30)

        button2 = tk.Button(self, text="SOLVER",
                            command=lambda: controller.show_frame(Solver))
        button2.pack(side="left", padx=5, pady=5, ipadx=30)

        button3 = tk.Button(self, text="SIMULATION",
                            command=lambda: controller.show_frame(Simulation))
        button3.pack(side="left", padx=5, pady=5, ipadx=30)

        button4 = tk.Button(self, text="POST-PROCESSING",
                            command=lambda: controller.show_frame(Postprocessing))
        button4.pack(side="left", padx=5, pady=5, ipadx=30)

        boutonc_1 = tk.Button(self, font=("Arial", 10), text="RUN SOLVER", background='#1c2028', fg='white',
                              borderwidth=2, activebackground='#202124', activeforeground='#1195cf', relief='ridge',
                              command='')
        boutonc_1.place(x=150, y=600)

        boutonc_2 = tk.Button(self, font=("Arial", 10), text="CONTROLE GENERATE FILE", background='#1c2028',
                              fg='white',
                              borderwidth=2, activebackground='#202124', activeforeground='#1195cf', relief='ridge',
                              command=save_info)
        boutonc_2.place(x=300, y=600)


class Solver(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="", )
        label.pack(side="bottom", pady=320, padx=100)
        labelsolv1 = tk.Label(self, font=("Arial", 10),
                              text="TIME INTEGRATION:")
        labelsolv1.place(x=50, y=100)
        listeProduits = ["Runge-Kutta", "Explicite EULER"]
        # 3) - Création de la Combobox via la méthode ttk.Combobox()
        listeCombo = ttk.Combobox(self, values=listeProduits)
        # 4) - Choisir l'élément qui s'affiche par défaut
        listeCombo.current(0)
        listeCombo.place(x=300, y=100)
        labelsolv2 = tk.Label(self, font=("Arial", 10),
                              text="CFL:", )
        labelsolv2.place(x=50, y=200)
        esolv1 = tk.StringVar()
        entrysolv2 = tk.Entry(self, textvariable=esolv1, width="50")
        entrysolv2.place(x=300, y=200)
        labelsolv3 = tk.Label(self, font=("Arial", 10),
                              text="MAXIMAL RESIDUEL:")
        labelsolv3.place(x=50, y=300)
        esolv2 = tk.StringVar()
        entrysolv3 = tk.Entry(self, textvariable=esolv2, width="30")
        entrysolv3.place(x=300, y=300)

        labelsolv4 = tk.Label(self, font=("Arial", 10),
                              text="MAX ITERACTIONS NUMBER:")
        labelsolv4.place(x=50, y=400)
        esolv3 = tk.StringVar()
        entrysolv4 = tk.Entry(self, textvariable=esolv3, width="30")
        entrysolv4.place(x=300, y=400)

        labelsolv5 = tk.Label(self, font=("Arial", 10),
                              text="NUMBER OF THREA:")
        labelsolv5.place(x=50, y=500)
        esolv4 = tk.StringVar()
        entrysolv5 = tk.Entry(self, textvariable=esolv4, width="30")
        entrysolv5.place(x=300, y=500)

        def save_info():
            solventry2_info = esolv1.get()
            solventry3_info = esolv2.get()
            solventry4_info = esolv3.get()
            solventry5_info = esolv4.get()
            print(solventry2_info)
            print(solventry3_info)
            print(solventry4_info)
            print(solventry5_info)
            file = open("fichier_test.txt", "w")
            file.write(f"------------------- SOLVER CONTROL -------------------\n\n"
                       "TIME INTEGRATION: " + "\n"
                       "CFL: " + solventry2_info,"\n"
                       "MAXIMAL RESIDUEL: " + solventry3_info, "\n"
                       "MAX ITERACTIONS NUMBER: " + solventry4_info, "\n"
                       "NUMBER OF THREA: " + solventry5_info, "\n")
            file.write("\n")
            file.close()

        button1 = tk.Button(self, text="PRE-PROCESSING",
                            command=lambda: controller.show_frame(Preprocessing))
        button1.pack(side="left", padx=5, pady=5, ipadx=30)

        button2 = tk.Button(self, text="SOLVER",
                            command=lambda: controller.show_frame(Solver))
        button2.pack(side="left", padx=5, pady=5, ipadx=30)

        button3 = tk.Button(self, text="SIMULATION",
                            command=lambda: controller.show_frame(Simulation))
        button3.pack(side="left", padx=5, pady=5, ipadx=30)

        button4 = tk.Button(self, text="POST-PRECESSING",
                            command=lambda: controller.show_frame(Postprocessing))
        button4.pack(side="left", padx=5, pady=5, ipadx=30)

        boutonc_1 = tk.Button(self, font=("Arial", 10), text="RUN SOLVER", background='#1c2028', fg='white',
                              borderwidth=2, activebackground='#202124', activeforeground='#1195cf', relief='ridge',
                              command='')
        boutonc_1.place(x=150, y=600)

        boutonc_2 = tk.Button(self, font=("Arial", 10), text="CONTROLE GENERATE FILE", background='#1c2028',
                              fg='white',
                              borderwidth=2, activebackground='#202124', activeforeground='#1195cf', relief='ridge',
                              command=save_info)
        boutonc_2.place(x=300, y=600)


class Simulation(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="")
        label.pack(side="bottom", pady=320, padx=100)
        labelsimul1 = tk.Label(self, font=("Arial", 10),
                               text="Speed Type:")
        labelsimul1.place(x=50, y=50)

        listeProduits = ["Mach", "Aoa"]
        # 3) - Création de la Combobox via la méthode ttk.Combobox()
        listeCombo1 = ttk.Combobox(self, values=listeProduits)
        # 4) - Choisir l'élément qui s'affiche par défaut
        listeCombo1.current(0)
        listeCombo1.place(x=300, y=50)

        labelsimul2 = tk.Label(self, font=("Arial", 10),
                               text="SPEED VALUE:")
        labelsimul2.place(x=50, y=100)
        esimul1 = tk.StringVar()
        entrysimul2 = tk.Entry(self, textvariable=esimul1, width="30")
        entrysimul2.place(x=300, y=100)

        labelsimul3 = tk.Label(self, font=("Arial", 10),
                               text="ANGLE OF ATTAQUE:")
        labelsimul3.place(x=50, y=150)
        esimul2 = tk.StringVar
        entrysimul3 = tk.Entry(self, textvariable=esimul2, width="30")
        entrysimul3.place(x=300, y=150)

        labelsimul4 = tk.Label(self, font=("Arial", 10),
                               text="Deg")
        labelsimul4.place(x=500, y=150)

        labelsimul5 = tk.Label(self, font=("Arial", 10),
                               text="PRESSURE:")
        labelsimul5.place(x=50, y=200)
        esimul3 = tk.StringVar()
        entrysimul5 = tk.Entry(self, textvariable=esimul3, width="30")
        entrysimul5.place(x=300, y=200)

        labelsimul6 = tk.Label(self, font=("Arial", 10),
                               text="Pa")
        labelsimul6.place(x=500, y=200)

        labelsimul7 = tk.Label(self, font=("Arial", 10),
                               text="VISCOSITY:")
        labelsimul7.place(x=50, y=250)
        esimul4 = tk.StringVar()
        entrysimul6 = tk.Entry(self, textvariable=esimul4, width="30")
        entrysimul6.place(x=300, y=250)

        labelsimul8 = tk.Label(self, font=("Arial", 10),
                               text="Ns/m^2")
        labelsimul8.place(x=500, y=250)

        labelsimul9 = tk.Label(self, font=("Arial", 10),
                               text="DENSITY:")
        labelsimul9.place(x=50, y=300)
        esimul5 = tk.StringVar()
        entrysimul7 = tk.Entry(self, textvariable=esimul5, width="30")
        entrysimul7.place(x=300, y=300)

        labelsimul10 = tk.Label(self, font=("Arial", 10),
                                text="Kg/m^3")
        labelsimul10.place(x=500, y=300)

        labelsimul11 = tk.Label(self, font=("Arial", 10),
                                text="GAMMA:")
        labelsimul11.place(x=50, y=350)
        esimul6 = tk.StringVar()
        entrysimul8 = tk.Entry(self, textvariable=esimul6, width="30")
        entrysimul8.place(x=300, y=350)

        labelsimul12 = tk.Label(self, font=("Arial", 10),
                                text="GAS CONSTANT:")
        labelsimul12.place(x=50, y=400)
        esimul7 = tk.StringVar()
        entrysimul9 = tk.Entry(self, textvariable=esimul7, width="50")
        entrysimul9.place(x=300, y=400)

        labelsimul13 = tk.Label(self, font=("Arial", 10),
                                text="J/Kg.X")
        labelsimul13.place(x=500, y=400)

        labelsimul14 = tk.Label(self, font=("Arial", 10),
                                text="SPECIFIC HEATH:")
        labelsimul14.place(x=50, y=450)
        esimul8 = tk.StringVar()
        entrysimul10 = tk.Entry(self, textvariable=esimul8, width="30")
        entrysimul10.place(x=300, y=450)

        labelsimul15 = tk.Label(self, font=("Arial", 10),
                                text="J/Kg.K")
        labelsimul15.place(x=500, y=450)

        def save_info():
            simentry1_info = esimul1.get()
            simentry2_info = esimul2.get()
            simentry3_info = esimul3.get()
            simentry4_info = esimul4.get()
            simentry5_info = esimul5.get()
            simentry6_info = esimul6.get()
            simentry7_info = esimul7.get()
            simentry8_info = esimul8.get()
            print(simentry1_info)
            print(simentry2_info)
            print(simentry3_info)
            print(simentry4_info)

            print(simentry5_info)
            print(simentry6_info)
            print(simentry7_info)
            print(simentry8_info)

            file = open("fichier_test.txt", "w")
            file.write(f"------------------- SIMULATION CONTROL -------------------\n\n"
                       "Speed Type: " + "\n"
                       "SPEED VALUE: " + simentry1_info,"\n"
                       "ANGLE OF ATTAQUE: " + simentry2_info + "Deg", "\n"
                       "PRESSURE: " + simentry3_info + "Pa","\n"
                       "VISCOSITY: " + simentry4_info + "Ns/m^2" , "\n"
                       "DENSITY: " + simentry5_info + "Kg/m^3" , "\n"
                       "GAMMA: " + simentry6_info, "\n"
                       "GAS CONSTANT: " + simentry7_info + "J/Kg.X", "\n"
                       "SPECIFIC HEATH: " + simentry8_info + "J/Kg.K", "\n")
            file.write("\n")
            file.close()

        button1 = tk.Button(self, text="PRE-PROCESSING",
                            command=lambda: controller.show_frame(Preprocessing))
        button1.pack(side="left", padx=5, pady=5, ipadx=30)

        button2 = tk.Button(self, text="SOLVER",
                            command=lambda: controller.show_frame(Solver))
        button2.pack(side="left", padx=5, pady=5, ipadx=30)

        button3 = tk.Button(self, text="SIMULATION",
                            command=lambda: controller.show_frame(Simulation))
        button3.pack(side="left", padx=5, pady=5, ipadx=30)

        button4 = tk.Button(self, text="POST-PRECESSING",
                            command=lambda: controller.show_frame(Postprocessing))
        button4.pack(side="left", padx=5, pady=5, ipadx=30)

        boutonc_1 = tk.Button(self, font=("Arial", 10), text="RUN SOLVER", background='#1c2028', fg='white',
                              borderwidth=2, activebackground='#202124', activeforeground='#1195cf', relief='ridge',
                              command='')
        boutonc_1.place(x=150, y=600)

        boutonc_2 = tk.Button(self, font=("Arial", 10), text="CONTROLE GENERATE FILE", background='#1c2028',
                              fg='white',
                              borderwidth=2, activebackground='#202124', activeforeground='#1195cf', relief='ridge',
                              command=save_info)
        boutonc_2.place(x=300, y=600)


class Postprocessing(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="")
        label.pack(side="bottom", pady=320, padx=100)
        labelpostproces1 = tk.Label(self, font=("Arial", 10),
                                    text="Pressure File:")
        labelpostproces1.place(x=50, y=100)
        epostpro1 = tk.StringVar()
        entrypostproces1 = tk.Entry(self, textvariable=epostpro1, width="50")
        entrypostproces1.place(x=200, y=100)

        open_buttonpost1 = HoverButton(self, font=("Arial", 10), text="...", command=ReadFile1)
        open_buttonpost1.place(x=550, y=100)

        labelprocess2 = tk.Label(self, font=("Arial", 10),
                                 text="RESIDUAL FILE:")
        labelprocess2.place(x=50, y=200)
        epostpro2 = tk.StringVar()
        entrypostproces2 = tk.Entry(self, textvariable=epostpro2, width="50")
        entrypostproces2.place(x=200, y=200)

        open_buttonpost2 = HoverButton(self, font=("Arial", 10), text="...", command=ReadFile1)
        open_buttonpost2.place(x=550, y=200)

        labelprocess3 = tk.Label(self, font=("Arial", 10),
                                 text="OUTPUT VTK FILE:")
        labelprocess3.place(x=50, y=300)
        epostpro3 = tk.StringVar()
        entrypostproces3 = tk.Entry(self, textvariable=epostpro3, width="50")
        entrypostproces3.place(x=200, y=300)

        open_buttonpost3 = HoverButton(self, font=("Arial", 10), text="...", command=ReadFile1)
        open_buttonpost3.place(x=550, y=300)

        def save_info():
            proentry1_info = epostpro1.get()
            proentry2_info = epostpro2.get()
            proentry3_info = epostpro3.get()
            print(proentry1_info)
            print(proentry2_info)
            print(proentry3_info)

            file = open("fichier_test.txt", "w")
            file.write(f"------------------- POST-PROCESSING CONTROL -------------------\n\n"
                       "Pressure File: " + proentry1_info, "\n"
                       "RESIDUAL FILE: " + proentry2_info, "\n"
                       "OUTPUT VTK FILE: " + proentry3_info,"\n")
            file.write("\n")
            file.close()

        button1 = tk.Button(self, text="PRE-PROCESSING",
                            command=lambda: controller.show_frame(Preprocessing))
        button1.pack(side="left", padx=5, pady=5, ipadx=30)

        button2 = tk.Button(self, text="SOLVER",
                            command=lambda: controller.show_frame(Solver))
        button2.pack(side="left", padx=5, pady=5, ipadx=30)

        button3 = tk.Button(self, text="SIMULATION",
                            command=lambda: controller.show_frame(Simulation))
        button3.pack(side="left", padx=5, pady=5, ipadx=30)

        button4 = tk.Button(self, text="POST-PRECESSING",
                            command=lambda: controller.show_frame(Postprocessing))
        button4.pack(side="left", padx=5, pady=5, ipadx=30)

        boutonc_1 = tk.Button(self, font=("Arial", 10), text="RUN SOLVER", background='#1c2028', fg='white',
                              borderwidth=2, activebackground='#202124', activeforeground='#1195cf', relief='ridge',
                              command='')
        boutonc_1.place(x=150, y=600)

        boutonc_2 = tk.Button(self, font=("Arial", 10), text="CONTROLE GENERATE FILE", background='#1c2028',
                              fg='white',
                              borderwidth=2, activebackground='#202124', activeforeground='#1195cf', relief='ridge',
                              command=save_info)
        boutonc_2.place(x=300, y=600)



app = SeaofBTCapp()
app.mainloop()
