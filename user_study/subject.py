# TODO:
#  - remove HTTP reqs
#  - get NTP offset at startup, and use this in timestamps

import Tkinter, urllib, urllib2, time, os, ntplib
import constants

class Subject(object):

    def __init__(self):
        instructions = "Subject\n\nWhenever you see a Right or Left hand signal,\n quickly press the corresponding arrow key.\nPress 'x' to exit."

        # Tkinter setup.
        self.root = Tkinter.Tk()
        self.root.bind("<Key>", self.keyPressed)
        label1 = Tkinter.Label(master = self.root, text = instructions)
        label1.pack()

        # Setup timing.
        client = ntplib.NTPClient()
        req = client.request(constants.ntpHost)
        self.offset = req.offset

        # Open log file.
        logName = os.path.join(constants.logDir, "subject-%d.log" % time.time())
        self.log = open(logName, "w")

    # Starts the experiement.
    def start(self):
        self.log.write("EXPERIMENTER\n")
        self.log.write("OFFSET %0.10f \n\n" % self.offset)
        self.log.write("%d - START\n" % time.time())
        self.root.mainloop()

    # Ends the experiment.
    def end(self):
        self.log.write("%d - END\n" % time.time())
        self.log.close()
        self.root.destroy()

    # Reacts to a key press.
    def keyPressed(self, event):
        if event.keysym in constants.signals:
            self.log.write("%d - SIGNAL: %s\n" % (time.time(), event.keysym))
        elif event.keysym == 'x':
            self.end()

if __name__ == '__main__':
    Subject().start()
