import math

fmodel = open ("model.txt", 'r')

def ReadFile (f, s):

    if s == "Transition":

        d = {"II": 0, "Is": 0, "sS": 0, "SG": 0, "GG": 0, "Ge": 0,
             "eE": 0, "EI": 0}

    else:

        d = {"AA": 0, "AC": 0, "AG": 0, "AT": 0, "CA": 0, "CC": 0, "CG": 0,
             "CT": 0, "GA": 0, "GC": 0, "GG": 0, "GT": 0, "TA": 0, "TC": 0,
             "TG": 0, "TT": 0}

    while True:

        x = f.readline ()

        if x == '\n': break
        if not x: break

        spaceIndex = x.find (' ')
        valueIndex = x.find ("= ")

        if valueIndex == -1: continue

        tmp = x[0: spaceIndex]

        for y in d.keys ():

            if y == tmp:
                d[y] = float (x[valueIndex + 2: len (x) - 1])
                d[y] = math.log (d[y])

    return d

def MultiLinESingleLine (f):

    f.readline ()
    tmp = f.readlines ()
    seq = str ("")

    for i in range (0, len (tmp)):
        seq += tmp[i][0: len (tmp[i]) - 1]

    return seq

def Viterbi (seq, transition, gene, intergene, s, S, e, E):

    #seq = seq[0: 100000]
    deltaPrev = {'I': 0.0, 's': 0.0, 'S': 0.0, 'G': 0.0, 'e': 0.0, 'E': 0.0}
    deltaNow = {'I': 0.0, 's': 0.0, 'S': 0.0, 'G': 0.0, 'e': 0.0, 'E': 0.0}
    backTrace = {'I': str (""), 's': str (""), 'S': str (""),
                 'G': str (""), 'e': str (""), 'E': str ("")}

    initTransitionProb = math.log (float (1/6))
    initEmissiionProb = math.log (float (1/16))

    for x in deltaPrev.keys ():

        deltaNow[x] = initTransitionProb + initEmissiionProb

    for x in backTrace.keys ():

        backTrace[x] += x

    induct = str ("")

    # Induction
    for i in range (1, len (seq)):

        for x in deltaPrev.keys ():

            deltaPrev[x] = deltaNow[x]

        # I State
        maxValue = deltaPrev['I'] + transition['II']
        backTrace['I'] += 'I'

        if maxValue < (deltaPrev['E'] + transition['EI']):

            backTrace['I'] = backTrace['I'][0: len (backTrace['I']) - 1]
            maxValue = deltaPrev['E'] + transition['EI']
            backTrace['I'] += 'E'

        deltaNow['I'] = maxValue + intergene[seq[i - 1: i + 1]]

        # s State
        maxValue = deltaPrev['I'] + transition['Is']
        backTrace['s'] += 'I'

        '''
        if maxValue < (deltaPrev['I'] + transition['Is']):

            maxValue = deltaPrev['I'] + transition['Is']
            backTrace['s'][len (backTrace['s'] - 1)] = 'I'
        '''
        deltaNow['s'] = maxValue + s[seq[i - 1: i + 1]]

        # S State
        maxValue = deltaPrev['s'] + transition['sS']
        backTrace['S'] += 's'

        deltaNow['S'] = maxValue + S[seq[i - 1: i + 1]]

        # G State
        maxValue = deltaPrev['G'] + transition['GG']
        backTrace['G'] += 'G'

        if maxValue < (deltaPrev['S'] + transition['SG']):

            backTrace['G'] = backTrace['G'][0: len (backTrace['G']) - 1]
            maxValue = deltaPrev['S'] + transition['SG']
            backTrace['G'] += 'S'

        deltaNow['G'] = maxValue + gene[seq[i - 1: i + 1]]

        #e State
        maxValue = deltaPrev['G'] + transition['Ge']
        backTrace['e'] += 'G'

        deltaNow['e'] = maxValue + e[seq[i - 1: i + 1]]

        #E
        maxValue = deltaPrev['e'] + transition['eE']
        backTrace['E'] += 'e'

        deltaNow['E'] = maxValue + E[seq[i - 1: i + 1]]

    # Terminate
    maxValue = max (deltaNow.values ())

    for x in deltaNow.keys ():

        if maxValue == deltaNow[x]:

            y = x
            break

    induct += y
    end = y

    # Back Trace
    for i in range (len (seq) - 1, -1, -1):

        induct += backTrace[end][i]
        end = induct[len (induct) - 1]

    induct = induct[:: -1]

    return induct

def PrintFile (f, seq, ind):

    f.write ("Gene region=\n")
    # Induction
    for i in range (1, len (ind) - 1):

        if ind[i - 1: i + 1] == 'SG':

            f.write (str (i) + ' ')
            geneRegionStart = i

        elif ind[i - 1: i + 1] == 'Ge':

            f.write (str (i) + ' ')
            geneRegionEnd = i
            f.write (seq[geneRegionStart: geneRegionEnd + 1] + '\n')

        else: continue

transitionLogProb = ReadFile (fmodel, "Transition")
emissionGeneLogProb = ReadFile (fmodel, 0)
emissionintergeneLogProb = ReadFile (fmodel, 0)
emissionsLogProb = ReadFile (fmodel, 0)
emissionSLogProb = ReadFile (fmodel, 0)
emissioneLogProb = ReadFile (fmodel , 0)
emissionELogProb = ReadFile (fmodel, 0)

ftest = open ("test.fasta", 'r')
fresult = open ("result.txt", 'w')

sequence = MultiLinESingleLine(ftest)
indudction = Viterbi (sequence, transitionLogProb, emissionGeneLogProb,
                      emissionintergeneLogProb, emissionsLogProb,
                      emissionSLogProb, emissioneLogProb, emissionELogProb)
PrintFile (fresult, sequence, indudction)

fmodel.close ()
ftest.close ()
fresult.close ()
