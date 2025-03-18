# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

modes = ["IDLE","PWM","ITEST","HOLD","TRACK"] # array to help with printing the mode

# open serial and menu
import serial
ser = serial.Serial('COM3',230400)
print('Opening port: ')
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\ta: Read current sensor (ADC counts)\tb: Read current sensor (mA)') # '\t' is a tab
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
    ser.write(selection_endline.encode()); # .encode() turns the string into a char array
    
    # take the appropriate action
    # there is no switch() in python, using if elif instead
    if (selection == 'a'): # read current sensor (adc counts)
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
        pwmvalstr = input('What PWM value would you like [-100 to 100]? ') # duty cycle
        pwmval = int(pwmvalstr) # turn into int
        ser.write((str(pwmval)+'\n').encode()) # send the number
        if (pwmval < 0) :
            direction = "clockwise"
            pwmval = -1*pwmval
        else:
            direction = "counterclockwise"
        print(f'PWM has been set to {pwmval}% in the ' + direction + ' direction.\n')
    elif (selection == 'g'): 
        kpcstr = input('\nEnter your desired Kp current gain [recommended: ?]: ')
        kpc = float(kpcstr) # turn into float
        ser.write((str(kpc)+'\n').encode()) # send the number
        kicstr = input('Enter your desired Ki current gain [recommended: ?]: ')
        kic = float(kicstr) # turn into float
        ser.write((str(kic)+'\n').encode()) # send the number
        print(f'Sending Kp = {kpcstr} and Ki = {kicstr} to the current controller.\n')
    elif (selection == 'h'):
        kpcstr = ser.read_until(b'\n')
        kpc = float(kpcstr) # turn into float
        kicstr = ser.read_until(b'\n')
        kic = float(kicstr) # turn into float
        print(f'The current controller is using Kp = {kpc} and Ki = {kic}.\n')
    elif (selection == 'i'): # set position gains
        kppstr = input('\nEnter your desired Kp position gain [recommended: 4.76]: ')
        kipstr = input('\nEnter your desired Ki position gain [recommended: 0.32]: ')
        kdpstr = input('\nEnter your desired Kd position gain [recommended: 10.63]: ')
        print(f'Sending Kp = {kppstr}, Ki = {kipstr}, and Kd = {kdpstr} to the position controller.\n')
    elif (selection == 'j'): 
        print(f'The position controller is using Kp = {kppstr}, Ki = {kipstr}, and Kd = {kdpstr}.\n')
    elif (selection == 'k'):
        print(' ')
    elif (selection == 'l'):
        mtranglestr = input('\nEnter the desired motor angle in degrees: ')
        print(f'Motor moving to {mtranglestr} degrees.')
    elif (selection == 'm'):
        steptrajstr = input('\nEnter step trajectory, in sec and degrees [time1, ang1: time2, ang2; ...]:\n\t')
    elif (selection == 'n'):
        cubictrajstr = input('\nEnter cubic trajectory, in sec and degrees [time1, ang1: time2, ang2; ...]:\n\t')
        print('Plotting the desired trajectoryand sending to PIC32 ... ')
        print('completed.')
    elif (selection == 'o'):
        print('Exiting client')
    elif (selection == 'p'): # Switch to IDLE mode
        print('Switching to IDLE mode')
    elif (selection == 'q'):
        print('Exiting client')
        has_quit = True; # exit client
        # be sure to close the port
        ser.close()
    elif (selection == 'r'):
        n_str = ser.read_until(b'\n') # get the incremented number back
        n_int = int(n_str) # turn it into an int
        print('The PIC32 controller mode is currently ' + str(modes[n_int])+'.\n')
    else:
        print('Invalid Selection ' + selection_endline)



