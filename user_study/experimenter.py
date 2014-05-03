import time, Tkinter, random, os, ntplib
import constants

class Experimenter(object):

    def __init__(self):
        self.started = False

        # Setup timing.
        client = ntplib.NTPClient()
        req = client.request(constants.ntpHost)
        self.offset = req.offset

        # Setup Tkinter.
        self.root = Tkinter.Tk()
        self.root.minsize(300, 300)
        self.root.bind("<Key>", self.keyPressed)
        label1 = Tkinter.Label(master = self.root, text = "Experimenter")
        label1.pack()
        self.text = Tkinter.StringVar()
        label2 = Tkinter.Label(master = self.root, textvariable = self.text)
        label2.pack()
        self.root.mainloop()

    # Prompts experimenter to do one signal.
    def promptSignal(self):
        if self.numSignals < constants.numSignals:
            self.signal = "Left" if self.signal == "Right" else "Right"
            self.text.set("Signal #%d. Do\n\n%s\n\nwhile pressing any key." % (self.numSignals, self.signal))
            self.numSignals += 1
        else:
            self.end()

    # Responds to any key press.
    def keyPressed(self, event):
        if not self.started:
            self.start()
        else:
            print self.signal

            # Clear the text.
            self.text.set("Hold signal...")

            # Log the signal.
            self.log.write("%.10f - SIGNAL: %s\n" % (time.time(), self.signal))
            self.root.after(constants.signalLength, self.endPrompt)


    # Sits idle between prompt.
    def endPrompt(self):
            # Clear the text.
            self.text.set("Waiting for next signal...")

            # Sleep.
            ms = random.randint(constants.sleepMin, constants.sleepMax)
            self.root.after(ms, self.promptSignal)

    # Starts a new experiment.
    def start(self):
        self.started = True

        # Open log file.
        logName = os.path.join(constants.logDir, "experimenter-%d.log" % time.time())
        self.log = open(logName, "w")

        # Other state.
        self.signal = random.choice(["Right", "Left"])
        self.numSignals = 0

        # Write log header.
        self.log.write("EXPERIMENTER\n")
        self.log.write("OFFSET %0.10f \n\n" % self.offset)
        self.log.write("%.10f - START\n" % time.time())
        self.promptSignal()

    # Ends an experiment.
    def end(self):
        self.started = False
        self.log.write("%.10f - END\n" % time.time())
        self.log.close()

if __name__ == '__main__':
    Experimenter()
