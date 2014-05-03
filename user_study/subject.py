import Tkinter, time, os, ntplib
import constants

class Subject(object):

    def __init__(self):
        instructions = "Subject\n\nWhenever you see a Right or Left hand signal,\n quickly press the corresponding arrow key.\nPress SPACE to begin.\nPress 'r' to reset."
        self.started = False

        # Setup timing.
        client = ntplib.NTPClient()
        req = client.request(constants.ntpHost)
        self.offset = req.offset

        # Tkinter setup.
        self.root = Tkinter.Tk()
        self.root.minsize(300, 300)
        self.root.bind("<Key>", self.keyPressed)
        Tkinter.Label(master = self.root, text = instructions).pack()
        self.text = Tkinter.StringVar()
        self.text.set("Not recording")
        Tkinter.Label(master = self.root, textvariable = self.text).pack()
        self.root.mainloop()

    # Starts the experiement.
    def start(self):
        self.started = True

        # Display that we've started.
        self.text.set("Recording...")

        # Open log file.
        logName = os.path.join(constants.logDir, "subject-%d.log" % time.time())
        self.log = open(logName, "w")

        # Write header to log
        self.log.write("SUBJECT\n")
        self.log.write("OFFSET %0.10f \n\n" % self.offset)
        self.log.write("%.10f - START\n" % time.time())

    # Ends the experiment.
    def end(self):
        self.started = False
        self.text.set("Not recording")
        self.log.write("%.10f - END\n" % time.time())
        self.log.close()

    # Reacts to a key press.
    def keyPressed(self, event):
        print event.keysym
        if event.keysym in constants.signals:
            if self.started:
                self.log.write("%.10f - SIGNAL: %s\n" % (time.time(), event.keysym))
        elif event.keysym == 'space':
            if not self.started:
                self.start()
        elif event.keysym == 'r':
            if self.started:
                self.end()

if __name__ == '__main__':
    Subject()
