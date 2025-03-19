# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

# imports
from genref import genRef
from statistics import mean
import matplotlib.pyplot as plt
import serial

# array to help with printing the mode
modes = ["IDLE", "PWM", "ITEST", "HOLD", "TRACK"]

# for plotting


def read_plot_matrix():
    n_str = ser.read_until(b'\n')  # get the number of data points to receive
    n_int = int(n_str)  # turn it into an int
    print('Data length = ' + str(n_int))
    ref = []
    data = []
    data_received = 0
    while data_received < n_int:
        # get the data as a string, ints seperated by spaces
        dat_str = ser.read_until(b'\n')
        dat_f = list(map(float, dat_str.split()))  # now the data is a list
        ref.append(dat_f[0])
        data.append(dat_f[1])
        data_received = data_received + 1
    meanzip = zip(ref, data)
    meanlist = []
    for i, j in meanzip:
        meanlist.append(abs(i-j))
    score = mean(meanlist)
    t = range(len(ref))  # index array
    plt.plot(t, ref, 'r*-', t, data, 'b*-')
    plt.title('Score = ' + str(score))
    plt.ylabel('value')
    plt.xlabel('index')
    plt.show()


# open serial and menu
ser = serial.Serial('COM3', 230400)
print('Opening port: ')
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    # '\t' is a tab
    print('\ta: Read current sensor (ADC counts)\tb: Read current sensor (mA)')
    print('\tc: Read encoder (counts)\t\td: Read encoder (deg)')
    print('\te: Reset encoder\t\t\tf: Set PWM (-100 to 100)')
    print('\tg: Set current gains\t\t\th: Get current gains')
    print('\ti: Set position gains\t\t\tj: Get position gains')
    print('\tk: Test current control\t\t\tl: Go to angle (deg)')
    print('\tm: Load step trajectory\t\t\tn: Load cubic trajectory')
    print('\to: Execute trajectory\t\t\tp: Unpower the motor')
    print('\tq: Quit client\t\t\t\tr: Get mode')
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'

    # send the command to the PIC32
    # .encode() turns the string into a char array
    ser.write(selection_endline.encode())

    # take the appropriate action
    # there is no switch() in python, using if elif instead
    if (selection == 'a'):  # read current sensor (adc counts)
        n_str = ser.read_until(b'\n')
        num = int(n_str)
        print('The motor current is ' + str(num) + ' ADC counts.\n')
    elif (selection == 'b'):
        n_str = ser.read_until(b'\n')
        num = float(n_str)
        print('The motor current is ' + str(num) + ' mA.\n')
    elif (selection == 'c'):
        n_str = ser.read_until(b'\n')
        num = int(n_str)
        print('The motor angle is ' + str(num) + ' counts.\n')
    elif (selection == 'd'):
        n_str = ser.read_until(b'\n')
        num = float(n_str)
        print('The motor angle is ' + str(num) + ' degrees.\n')
    elif (selection == 'e'):
        print(' ')
    elif (selection == 'f'):
        pwmvalstr = input(
            'What PWM value would you like [-100 to 100]? ')  # duty cycle
        pwmval = int(pwmvalstr)  # turn into int
        ser.write((str(pwmval)+'\n').encode())  # send the number
        if (pwmval < 0):
            direction = "clockwise"
            pwmval = -1*pwmval
        else:
            direction = "counterclockwise"
        print(
            f'PWM has been set to {pwmval}% in the ' + direction + ' direction.\n')
    elif (selection == 'g'):
        kpcstr = input(
            '\nEnter your desired Kp current gain [recommended: 0.15]: ')
        kpc = float(kpcstr)  # turn into float
        ser.write((str(kpc)+'\n').encode())  # send the number
        kicstr = input('Enter your desired Ki current gain [recommended: 0.01]: ')
        kic = float(kicstr)  # turn into float
        ser.write((str(kic)+'\n').encode())  # send the number
        print(
            f'Sending Kp = {kpcstr} and Ki = {kicstr} to the current controller.\n')
    elif (selection == 'h'):
        kpcstr = ser.read_until(b'\n')
        kpc = float(kpcstr)  # turn into float
        kicstr = ser.read_until(b'\n')
        kic = float(kicstr)  # turn into float
        print(f'The current controller is using Kp = {kpc} and Ki = {kic}.\n')
    elif (selection == 'i'):  # set position gains
        kppstr = input(
            '\nEnter your desired Kp position gain [recommended: 4.76]: ')
        kpp = float(kppstr)  # turn into float
        ser.write((str(kpp)+'\n').encode())  # send the number
        kipstr = input(
            '\nEnter your desired Ki position gain [recommended: 0.32]: ')
        kip = float(kipstr)  # turn into float
        ser.write((str(kip)+'\n').encode())  # send the number
        kdpstr = input(
            '\nEnter your desired Kd position gain [recommended: 10.63]: ')
        kdp = float(kdpstr)  # turn into float
        ser.write((str(kdp)+'\n').encode())  # send the number
        print(
            f'Sending Kp = {kppstr}, Ki = {kipstr}, and Kd = {kdpstr} to the position controller.\n')
    elif (selection == 'j'):
        kppstr = ser.read_until(b'\n')
        kpp = float(kppstr)  # turn into float
        kipstr = ser.read_until(b'\n')
        kip = float(kipstr)  # turn into float
        kdpstr = ser.read_until(b'\n')
        kdp = float(kdpstr)  # turn into float
        print(
            f'The position controller is using Kp = {kpp}, Ki = {kip}, and Kd = {kdp}.\n')
    elif (selection == 'k'):
        read_plot_matrix()
        print(' ')
    elif (selection == 'l'):
        mtranglestr = input('\nEnter the desired motor angle in degrees: ')
        mtrangle = int(mtranglestr)
        ser.write((str(mtrangle)+'\n').encode())  # send the number
        print(f'Motor moving to {mtrangle} degrees.')
    elif (selection == 'm'):
        steptrajstr = input(
            '\nEnter step trajectory, in sec and degrees [time1, ang1: time2, ang2; ...]:\n\t')
    elif (selection == 'n'):
        cubictrajstr = input(
            '\nEnter cubic trajectory, in sec and degrees [time1, ang1: time2, ang2; ...]:\n\t')
        print('Plotting the desired trajectoryand sending to PIC32 ... ')
        print('completed.')
    elif (selection == 'o'):
        print('Exiting client')
    elif (selection == 'p'):  # Switch to IDLE mode
        print('Switching to IDLE mode\n')
    elif (selection == 'q'):
        print('Exiting client')
        has_quit = True  # exit client
        # be sure to close the port
        ser.close()
    elif (selection == 'r'):
        n_str = ser.read_until(b'\n')  # get the incremented number back
        n_int = int(n_str)  # turn it into an int
        print('The PIC32 controller mode is currently ' +
              str(modes[n_int])+'.\n')
    else:
        print('Invalid Selection ' + selection_endline)
