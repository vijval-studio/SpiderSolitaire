#!/usr/bin/env python3

import tkinter as tk
import subprocess
import threading

class SpiderGUI:
    def __init__(self, master):
        self.master = master
        self.master.title("Spider Solitaire GUI")

        self.columns = [[] for _ in range(10)]
        self.labels = [[] for _ in range(10)]

        self.src_col = None
        self.card_val = None

        self.card_font = ("Courier", 14)
        self.process = subprocess.Popen(
            ['./spider'],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1
        )

        self.canvas = tk.Frame(self.master)
        self.canvas.pack()

        self.stats_label = tk.Label(self.master, text="", font=("Arial", 12), fg="blue")  # NEW
        self.stats_label.pack()

        self.deal_button = tk.Button(self.master, text="Deal New Row", command=self.deal_new_row)
        self.deal_button.pack(anchor='se', padx=10, pady=10)

        self.message_label = tk.Label(self.master, text="", font=("Arial", 12), fg="red")
        self.message_label.pack()

        self.game_over_frame = tk.Frame(self.master)
        self.quit_button = tk.Button(self.game_over_frame, text="Quit", command=self.quit_game)
        self.restart_button = tk.Button(self.game_over_frame, text="New Game", command=self.restart_game)
        self.quit_button.pack(side='left', padx=10)
        self.restart_button.pack(side='left', padx=10)
        self.game_over_frame.pack(pady=10)
        self.game_over_frame.pack_forget()

        self.read_thread = threading.Thread(target=self.read_stdout, daemon=True)
        self.read_thread.start()

    def read_stdout(self):
        buffer = ""
        while True:
            line = self.process.stdout.readline()
            if not line:
                break

            buffer += line
            if "The game is finished" in buffer:
                self.show_game_over_buttons()
                self.display_message(buffer)
                self.update_stats(buffer)  # NEW
                buffer = ""
            elif line.strip() == "":
                self.parse_board(buffer)
                self.update_stats(buffer)  # NEW
                buffer = ""

    def update_stats(self, text):  # NEW FUNCTION
        move = ""
        time = ""
        for line in text.splitlines():
            if line.startswith("Move Count:"):
                move = line.strip()
            elif line.startswith("Time taken so far:"):
                time = line.strip()
        if move or time:
            self.stats_label.config(text=f"{move}\n{time}")

    def parse_board(self, text):
        lines = text.split('\n')
        self.columns = [[] for _ in range(10)]

        for row in lines:
            for col, val in enumerate(row.split("\t")):
                if val != "":
                    self.columns[col].append(val)

        self.redraw_board()

    def redraw_board(self):
        for widgets in self.labels:
            for label in widgets:
                label.destroy()
        self.labels = [[] for _ in range(10)]

        for col, stack in enumerate(self.columns):
            for row, val in enumerate(stack):
                if val != ' ':
                    lbl = tk.Label(self.canvas, text=val, font=self.card_font,
                                   width=4, bg="green" if val != "X" else "gray",
                                   fg="white", relief="raised", bd=2)
                else:
                    lbl = tk.Label(self.canvas, text=val, font=self.card_font,
                                   width=4, bg="light grey",
                                   fg="light grey", bd=0)

                lbl.grid(row=row, column=col, padx=3, pady=1)
                value_copy = val
                lbl.bind("<Button-1>", lambda e, c=col, v=value_copy: self.on_card_click(c, v))
                self.labels[col].append(lbl)

    def on_card_click(self, col, val):
        if self.src_col is None:
            if val == ' ':
                return
            self.src_col = col
            self.card_val = val
            for lbl in self.labels[col]:
                lbl.config(relief="sunken")
        else:
            if self.src_col != col:
                move = f"{self.card_val} {self.src_col} {col}\n"
                print(f"Sending move: {move.strip()}")
                self.process.stdin.write(move)
                self.process.stdin.flush()
            for lbl in self.labels[self.src_col]:
                lbl.config(relief="raised")
            self.src_col = None
            self.card_val = None

    def deal_new_row(self):
        self.process.stdin.write("0\n")
        self.process.stdin.flush()

    def show_game_over_buttons(self):
        self.game_over_frame.pack()
        self.deal_button.config(state='disabled')

    def hide_game_over_buttons(self):
        self.game_over_frame.pack_forget()
        self.deal_button.config(state='normal')

    def quit_game(self):
        self.process.stdin.write("q\n")
        self.process.stdin.flush()
        self.master.quit()

    def restart_game(self):
        self.process.stdin.write("r\n")
        self.process.stdin.flush()
        self.hide_game_over_buttons()
        self.message_label.config(text="")
        self.stats_label.config(text="")  # Clear stats too

    def display_message(self, msg):
        self.message_label.config(text=msg.strip())


root = tk.Tk()
app = SpiderGUI(root)
root.mainloop()