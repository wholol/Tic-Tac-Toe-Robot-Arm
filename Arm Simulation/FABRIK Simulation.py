import math
import pygame
import sys

class FABRIK:
    number_of_joints = 4
    posX_array = []         #stores the position of x coordiantes for the links
    posY_array = []         #stores the poisiton of the y coordinates for the links
    magnitude_array = []
    Lambda = []
    link_length_array = []
    target_posX = float(0)
    target_posY = float(0)

    def __init__(self, link1_posX, link1_posY, link2_posX, link2_posY, link3_posX, link3_posY, link4_posX, link4_posY, link_length_1, link_length_2,
                 link_length_3, tolerance):
        self.link_1_posX = float(link1_posX)        #coordinates of link 1 NOTE. this should be zero (origin)
        self.link_2_posX = float(link2_posX)        #coordinates of link 2
        self.link_3_posX = float(link3_posX)        #coordinates of link 3
        self.link_4_posX = float(link4_posX)        #coordinates of link 4

        self.link_1_posY = float(link1_posY)       # coordinates of link 1 NOTE, this should be zero (origin)
        self.link_2_posY = float(link2_posY)       # coordinates of link 2
        self.link_3_posY = float(link3_posY)       # coordinates of link 3
        self.link_4_posY = float(link4_posY)       # coordinates of link 4

        self.link_length_1 = float(link_length_1)  #length from link 1 to 2
        self.link_length_2 = float(link_length_2)  #lenth from link 2 to 3
        self.link_length_3 = float(link_length_3)  #length from link 3 to 4

        self.tolerance = float(tolerance)

    def set_target_pos(self,x,y):
        self.target_posX = float(x)
        self.target_posY = float(y)

    def initialize_arrays(self):                    #stores the coordinates of each link into an array
        self.posX_array.append(self.link_1_posX)            #initiilize position X array
        self.posX_array.append(self.link_2_posX)
        self.posX_array.append(self.link_3_posX)
        self.posX_array.append(self.link_4_posX)

        self.posY_array.append(self.link_1_posY)        #initialize position Y array
        self.posY_array.append(self.link_2_posY)
        self.posY_array.append(self.link_3_posY)
        self.posY_array.append(self.link_4_posY)

        self.link_length_array.append(self.link_length_1)   #initialize link length array
        self.link_length_array.append(self.link_length_2)
        self.link_length_array.append(self.link_length_3)

        self.magnitude_array.append(0)                      #initialize magnitude array (distance between two links)
        self.magnitude_array.append(0)
        self.magnitude_array.append(0)

        self.Lambda.append(0)                           #intiailzie lambda
        self.Lambda.append(0)
        self.Lambda.append(0)




    def printPosition(self):
        for i in range(0,3,1):
            print(self.posX_array[i])


    def printArrays(self):
        print('posX value','                   ','   posY value')
        for i in range(len(self.posY_array)):  # prints the coordinates (testing)
            print(self.posX_array[i], '                                ',self.posY_array[i],'                           joint number:', i)
            print(" ")


    def compute_target_magnitude(self):                             #computes the magnitude of the target relative to the base
        return math.sqrt(math.pow(self.target_posX - self.posX_array[0],2) + math.pow(self.target_posY-self.posY_array[0],2))

    def link_magnitude_difference(self,i):
         #computes the magnitude of two links in the cartesian space
        return (math.sqrt(math.pow((self.posX_array[i + 1] - self.posX_array[i]),2) + math.pow((self.posY_array[i + 1] - self.posY_array[i]),2)))

    def compute_difference(self):       #computes difference between target and end effector
        return math.sqrt(math.pow(self.target_posX - self.posX_array[3],2) + math.pow(self.target_posY - self.posY_array[3],2))

    def check_max_distance(self):
        max_distance = self.link_length_1 + self.link_length_2 + self.link_length_3
        if max_distance < self.compute_target_magnitude():
            return False

    def printAngles(self):      #this function calcualtes the angles needed
        theta_3 = (-math.atan2(self.posY_array[3] - self.posY_array[2], self.posX_array[3] - self.posX_array[2])) * (
                    180 / 3.14159)
        theta_2 = (-math.atan2(self.posY_array[2] - self.posY_array[1], self.posX_array[2] - self.posX_array[1])) * (
                      180 / 3.14159)

        theta_1 = (-math.atan2(self.posY_array[1] - self.posY_array[0], self.posX_array[1] - self.posX_array[0]))*(
                      180 / 3.14159)

        print(theta_1, '   ',theta_2,'   ',theta_3)

    def compute_algorithm(self):
        self.compute_target_magnitude()     #compute the target magnitude

        if (self.check_max_distance() == False):
            print("target value is too large.")
            return

        while(1):

            difference = float(math.fabs(self.compute_difference()))
            self.posX_array[3] = self.target_posX   #set end effector position to target
            self.posY_array[3] = self.target_posY   #set end effector position to target
            for i in range(2,-1,-1):
                self.magnitude_array[i] = self.link_magnitude_difference(i) #compute the difference in magnitude between two links
                self.Lambda[i] = self.link_length_array[i]/self.magnitude_array[i] #compute the ratio lambda (ratio of link length to the distance)
                self.posX_array[i] = (1 - self.Lambda[i]) * self.posX_array[i + 1] + self.Lambda[i] * self.posX_array[i]  # update the point to the new point via lambda ratio
                self.posY_array[i] = (1 - self.Lambda[i]) * self.posY_array[i + 1] + self.Lambda[i] * self.posY_array[i]
            """
            self.printArrays()
            print('difference is: ', difference)
            print(" ")
            print(" ")
            """
            self.printAngles()

            self.posX_array[0] = link1_posX  # reset the base position to 0
            self.posY_array[0] = link1_posY  # reset base position to 0

            #foward difference (backward reaching)
            for j in range(0,3,1):
                self.magnitude_array[j] = self.link_magnitude_difference(j)
                self.Lambda[j] = self.link_length_array[j] / self.magnitude_array[j]
                self.posX_array[j+1] = (1 - self.Lambda[j]) * self.posX_array[j] + self.Lambda[j] * self.posX_array[j+1]
                self.posY_array[j+1] = (1 - self.Lambda[j]) * self.posY_array[j] + self.Lambda[j] * self.posY_array[j+1]


            difference = float(math.fabs(self.compute_difference()))

            """
            self.printArrays()
            print('difference is: ', difference)
            print(" ")
            print(" ")
            """


            if difference < self.tolerance:     #if the diffeence is smaller
                break



    #parameters


#link1_posX = 100
#link1_posY = 50
#link2_posX = 100
#link2_posY = 200
#link3_posX = 100
#link3_posY = 250
#link4_posX = 100
#link4_posY = 350

link1_posX = 0
link1_posY = 70
link2_posX = 32.3
link2_posY = 120
link3_posX = 64.7
link3_posY = 241
link4_posX = 111.29
link4_posY = 415

link_length_1 = 125
link_length_2 = 125
link_length_3 = 180
tolerance = 0.05
#parameters

fab = FABRIK(link1_posX, link1_posY, link2_posX, link2_posY,link3_posX, link3_posY, link4_posX, link4_posY , link_length_1, link_length_2,
                 link_length_3,tolerance)

white = ((255,255,255))
black = ((0,0,0))
pygame.init()
fab.initialize_arrays()
screen = pygame.display.set_mode((1000,1000))
screen.fill(white)



while 1:
    mouse = pygame.mouse.get_pos()

    point_b = [[fab.posX_array[0], fab.posY_array[0]], [fab.posX_array[1], fab.posY_array[1]],
              [fab.posX_array[2], fab.posY_array[2]], [fab.posX_array[3], fab.posY_array[3]]]

    pygame.draw.lines(screen, black, False,point_b, 4)
    pygame.draw.circle(screen, black, [int(fab.posX_array[0]), int(fab.posY_array[0])], 7)
    pygame.draw.circle(screen, black, [int(fab.posX_array[1]), int(fab.posY_array[1])], 7)
    pygame.draw.circle(screen, black, [int(fab.posX_array[2]), int(fab.posY_array[2])], 7)
    pygame.draw.circle(screen, black, [int(fab.posX_array[3]), int(fab.posY_array[3])], 7)


    fab.set_target_pos(mouse[0],mouse[1])
    fab.compute_algorithm()
    pygame.display.flip()

    pygame.time.delay(25)
    screen.fill(white)
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()















