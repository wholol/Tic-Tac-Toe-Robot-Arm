from math import inf
import serial, time
import random
TicTacArray = [ '-','-','-',        #array for Tic Tac Toe.
                '-','-','-',
                '-','-','-']
playersymbol = ['O','X']        #player symbols. for minimizng player, use playersymbol[0], for maximizer, use playersymbol[1] for minimizer
depth = 9
human_win = False
AI_win = False
draw = False
AI_pos = 60
arduino = serial.Serial('COM6', baudrate=115200, timeout=5) #initialize serial

#AI= maximizer, human is minimizer

def instructions():
    print('welcome to TicTacToe. You will be X.')
    queue = input("enter '1' if you want to start, enter '2' for AI to start first.:" "")

    return queue


def gamescore(child):      #checks for win,lose, and draw of each components
    return winstate(child)


def winstate(child):            #this function checks if the array wins,lose or draw in the tree (leaf nodes)
    #win cases for AI
    win = False
    if (child[0] == 'O' and child[1] == 'O' and child[2] == 'O'):
        win = True
        return [10 ,'over']
    if(child[3] == 'O' and child[4] == 'O' and child[5] == 'O'):
        win = True
        return [10, 'over']
    if (child[6] == 'O' and child[7] == 'O' and child[8] == 'O'):
        win = True
        return [10, 'over']
    if (child[0] == 'O' and child[4] == 'O' and child[8] == 'O'):
        win = True
        return [10, 'over']
    if (child[2] == 'O' and child[4] == 'O' and child[6] == 'O'):
        win = True
        return [10, 'over']
    if (child[0] == 'O' and child[3] == 'O' and child[6] == 'O'):
        win = True
        return [10, 'over']
    if (child[1] == 'O' and child[4] == 'O' and child[7] == 'O'):
        win = True
        return [10, 'over']
    if (child[3] == 'O' and child[5] == 'O' and child[8] == 'O'):
        win = True
        return [10, 'over']

    #win cases for human
    if (child[0] == 'X' and child[4] == 'X' and child[8] == 'X'):
        win = True
        return  [-10, 'over']
    if (child[2] == 'X' and child[4] == 'X' and child[6] == 'X'):
        win = True
        return  [-10, 'over']
    if (child[0] == 'X' and child[1] == 'X' and child[2] == 'X'):
        win = True
        return  [-10, 'over']
    if (child[3] == 'X' and child[4] == 'X' and child[5] == 'X'):
        win = True
        return  [-10, 'over']
    if (child[6] == 'X' and child[7] == 'X' and child[8] == 'X'):
        win = True
        return  [-10, 'over']
    if (child[0] == 'X' and child[3] == 'X' and child[6] == 'X'):
        win = True
        return  [-10, 'over']
    if (child[1] == 'X' and child[4] == 'X' and child[7] == 'X'):
        win = True
        return  [-10, 'over']
    if (child[3] == 'X' and child[5] == 'X' and child[8] == 'X'):
        win = True
        return  [-10, 'over']

    if (child.count('-') == 0 and win != True):
        return [0, 'over']
    else:
        return [1 ,'not over']

def gameover():                     #checks if won or draw for the REAL BOARD.
    global human_win
    global AI_win
    global draw

    if TicTacArray.count('-') == 0:
        draw = True

    #win cases for AI
    if (TicTacArray[0] == 'O' and TicTacArray[1] == 'O' and TicTacArray[2] == 'O'):
        AI_win = True

    if(TicTacArray[3] == 'O' and TicTacArray[4] == 'O' and TicTacArray[5] == 'O'):
        AI_win = True

    if (TicTacArray[6] == 'O' and TicTacArray[7] == 'O' and TicTacArray[8] == 'O'):
        AI_win = True

    if (TicTacArray[0] == 'O' and TicTacArray[4] == 'O' and TicTacArray[8] == 'O'):
        AI_win = True

    if (TicTacArray[2] == 'O' and TicTacArray[4] == 'O' and TicTacArray[6] == 'O'):
        AI_win = True

    if (TicTacArray[0] == 'O' and TicTacArray[3] == 'O' and TicTacArray[6] == 'O'):
        AI_win = True

    if (TicTacArray[1] == 'O' and TicTacArray[4] == 'O' and TicTacArray[7] == 'O'):
        AI_win = True

    if (TicTacArray[2] == 'O' and TicTacArray[5] == 'O' and TicTacArray[8] == 'O'):
        AI_win = True

    #win cases for human
    if (TicTacArray[0] == 'X' and TicTacArray[4] == 'X' and TicTacArray[8] == 'X'):
        human_win = True

    if (TicTacArray[2] == 'X' and TicTacArray[4] == 'X' and TicTacArray[6] == 'X'):
        human_win = True

    if (TicTacArray[0] == 'X' and TicTacArray[1] == 'X' and TicTacArray[2] == 'X'):
        human_win = True

    if (TicTacArray[3] == 'X' and TicTacArray[4] == 'X' and TicTacArray[5] == 'X'):
        human_win = True

    if (TicTacArray[6] == 'X' and TicTacArray[7] == 'X' and TicTacArray[8] == 'X'):
        human_win = True

    if (TicTacArray[0] == 'X' and TicTacArray[3] == 'X' and TicTacArray[6] == 'X'):
        human_win = True

    if (TicTacArray[1] == 'X' and TicTacArray[4] == 'X' and TicTacArray[7] == 'X'):
        human_win = True

    if (TicTacArray[2] == 'X' and TicTacArray[5] == 'X' and TicTacArray[8] == 'X'):
        human_win = True



def humanTurn():                    #player human turn
    global depth
    number = int(input("Human turn. please chose where you want to place."))
    while (number < 0 or number > 8):
        number = int(input("choose a valid spot only. Choose again: "))

    while (TicTacArray[number] != '-'):
        number = int(input('spot already used. Choose again: '))

    TicTacArray[number] = 'X'
    depth = depth - 1

def AITurn():  #AI turn
    global AI_pos
    global depth
    if depth == 9:
        first_move = random.randrange(0,8)
        TicTacArray[first_move] = 'O'
        return first_move


    else:
        ChildArray = TicTacArray.copy()  # array used for recursion
        number = minimax(ChildArray,depth,'MaximizingPlayer')
        TicTacArray[number[0]] = 'O'
        AI_pos =  number[0]            #return the position and pass it to serial buffer

    depth = depth - 1


def minimax(child, depth ,min_or_max):      #minimax: core function. contains the game tree for the tictactoe function
    check_status = gamescore(child)
    if depth == 0 or check_status[1] == 'over':        #base case, if the game is over (win lost or draw, return the the value)
        return [-1,check_status[0]]        #if the game tree has reached the leaf nodes, return the game status of the child.

    if (min_or_max == 'MinimizingPlayer'):           #if player is a min player, who wants to choose the smallest value.
        best_move_AI = [-1, +inf]
        for i in range(len(child)):   #loop through all the unfilled array elements (i.e. possible routes or child
            if child[i] == '-':       #if the spot is empty
                child[i] = playersymbol[1]         #maps the possible moves for the opponent, as the childs are the enemy
                score = minimax(child,depth - 1,'MaximizingPlayer')            #returns the score of the given child/path
                child[i] = '-'                                        #undo the move
                score[0] = i                                #store the position of the child in an array
                if score[1] < best_move_AI[1]:                 #score: if the nodes(child) score is smaller
                    best_move_AI = score                       #update the best_move to that score
        return best_move_AI                        #return the best move to update the score

    if (min_or_max == 'MaximizingPlayer'):          #if the player is the maximizer, who wants to choose the largest value
        best_move_human = [-1, -inf]
        for j in range(len(child)):
            if child[j] == '-':
                child[j] = playersymbol[0]
                score = minimax(child,depth - 1,'MinimizingPlayer')
                child[j] = '-'
                score[0] = j
                if score[1] > best_move_human[1]:
                    best_move_human = score
        return best_move_human

def ParsePosition():        #writes the data to arduino
    global AI_pos
    if (AI_pos == 0):
        arduino.write(b'0')
    if (AI_pos == 1):
        arduino.write(b'1')
    if (AI_pos == 2):
        arduino.write(b'2')
    if (AI_pos == 3):
        arduino.write(b'3')
    if (AI_pos == 4):
        arduino.write(b'4')
    if (AI_pos == 5):
        arduino.write(b'5')
    if (AI_pos == 6):
        arduino.write(b'6')
    if (AI_pos == 7):
        arduino.write(b'7')
    if (AI_pos == 8):
        arduino.write(b'8')


def PrintArray():       #prints the array
    print("|",TicTacArray[0] , "|", TicTacArray[1] , "|",TicTacArray[2],"|")
    print("|",TicTacArray[3], "|", TicTacArray[4], "|", TicTacArray[5], "|")
    print("|",TicTacArray[6], "|", TicTacArray[7], "|", TicTacArray[8], "|")

def main():         #main function

    time.sleep(5)  # give the connection a second to settle

    player_turn = instructions()        #print the instructions for player_turn
    PrintArray()                        #print the array of the tic tac toe

    if player_turn == '1':              #human first
        while 1:

            humanTurn()
            PrintArray()
            print(" ")
            print(" ")
            gameover()          #check for game_ovr status
            if human_win == True:
                print("human wins")
                break
            if draw == True:
                print("draw")
                break
            print("AI Turn")
            AITurn()
            print(AI_pos)
            ParsePosition()         #parse position to arduino
            print(arduino.readline().decode('ascii'))     #print arduino serial buffer

            PrintArray()
            print(" ")
            print(" ")
            gameover()
            if AI_win == True:
                print("AI wins")
                break
            if draw == True:
                print("draw")
                break

    if player_turn == '2':     #AI first
        while 1:
                AITurn()
                PrintArray()
                print(" ")
                print(" ")
                gameover()
                if (AI_win == True):
                    print("AI wins")
                    break
                if (draw == True):
                    print("draw")
                    break

                humanTurn()
                PrintArray()
                print(" ")
                print(" ")
                gameover()
                if human_win == True:
                    print("human wins")
                    break
                if draw == True:
                    print("draw")
                    break


main()



