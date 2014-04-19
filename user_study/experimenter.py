import time, Tkinter, random, os, ntplib
import constants

class Experimenter(object):

    def __init__(self):
        # Setup Tkinter.
        self.root = Tkinter.Tk()
        self.root.bind("<Key>", self.keyPressed)
        label1 = Tkinter.Label(master = self.root, text = "Experimenter")
        label1.pack()
        self.text = Tkinter.StringVar()
        label2 = Tkinter.Label(master = self.root, textvariable = self.text)
        label2.pack()

        # Setup timing.
        client = ntplib.NTPClient()
        req = client.request(constants.ntpHost)
        self.offset = req.offset

        # Open log file.
        logName = os.path.join(constants.logDir, "experimenter-%d.log" % time.time())
        self.log = open(logName, "w")

        # Other state.
        self.signal = None
        self.numSignals = 0

    # Prompts experimenter to do one signal.
    def promptSignal(self):
        if self.numSignals < constants.numSignals:
            self.signal = random.choice(constants.signals)
            self.text.set("Signal #%d. Do\n\n%s\n\nwhile pressing any key." % (self.numSignals, self.signal))
            self.numSignals += 1
        else:
            self.end()

    # Responds to any key press.
    def keyPressed(self, event):
        if self.signal:
            # Log the signal.
            self.log.write("%d - SIGNAL: %s\n" % (time.time(), self.signal))

            # Clear the text.
            self.text.set("Waiting for next signal...")
            self.signal = None

            # Sleep.
            ms = random.randint(constants.sleepMin, constants.sleepMax)
            self.root.after(ms, self.promptSignal)

    # Starts a new experiment.
    def start(self):
        self.log.write("EXPERIMENTER\n")
        self.log.write("OFFSET %0.10f \n\n" % self.offset)
        self.log.write("%d - START\n" % time.time())
        self.promptSignal()
        self.root.mainloop()

    # Ends an experiment.
    def end(self):
        self.log.write("%d - END\n" % time.time())
        self.log.close()
        self.root.destroy()

if __name__ == '__main__':
    Experimenter().start()
