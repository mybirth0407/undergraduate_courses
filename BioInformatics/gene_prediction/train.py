def Calculate (f):

    l = 0
    c = 0
    d = {"AA": 0, "AC": 0, "AG": 0, "AT": 0,
         "CA": 0, "CC": 0, "CG": 0, "CT": 0,
         "GA": 0, "GC": 0, "GG": 0, "GT": 0,
         "TA": 0, "TC": 0, "TG": 0, "TT": 0}

    while True:

        line = f.readline ()

        if not line: break

        elif line[0] == '>': continue

        l += len (line)
        c += 1

        for i in range (0, len (line)):

            if line[i: i + 2] in d:
                d[line[i: i + 2]] += 1

    return l, c, d

def EmissionProbCalculate (d):

    s = {"sumA": 0, "sumC": 0, "sumG": 0, "sumT": 0}

    for x in s.keys ():

        s[x] = d[x[3] + 'A'] + d[x[3] + 'C'] + d[x[3] + 'G'] + d[x[3] + 'T']

    for y in d.keys ():

        d[y] /= s["sum" + y[0]]

    return d

def TransitionProbCalculate (gL, gC, iL, iC):

    d = {"II": 0, "Is": 0, "sS": 1, "SG": 1,
         "GG": 0, "Ge": 0, "eE": 1, "EI": 1}

    d["Ge"] = gC / gL
    d["Is"] = (iC - 1) / (iL -1)
    d["GG"] = 1 - d["Ge"]
    d["II"] = 1 - d["Is"]

    return d

def PseudoValueInsert (n):

    d = {"AA": 0, "AC": 0, "AG": 0, "AT": 0,
         "CA": 0, "CC": 0, "CG": 0, "CT": 0,
         "GA": 0, "GC": 0, "GG": 0, "GT": 0,
         "TA": 0, "TC": 0, "TG": 0, "TT": 0}

    if n == "s":

        for x in d.keys ():

            d[x] = 0.000000000001

        d["AT"] = 0.999999999985

    elif n == "S":

        for x in d.keys ():

            d[x] = 0.000000000001

        d["TG"] = 0.999999999985

    elif n == "e":

        for x in d.keys ():

            d[x] = 0.000000000001

        d["TG"] = 0.999999999985

    elif n == "E":

        for x in d.keys ():

            d[x] = 0.000000000001

        d["GA"] = 0.999999999985

    return d

def PrintFile (f, d):

    for x in d.keys ():

        f.write (x)
        f.write (' ')
        f.write ('= ')
        f.write (str (d[x]))
        f.write ('\n')

fGenic = open ("train_genic.fasta", 'r')
fIntergenic = open ("train_intergenic.fasta", 'r')

geneLength, geneCount, G = Calculate (fGenic)
intergeneLength, intergeneCount, I = Calculate (fIntergenic)
emissionGeneProb = EmissionProbCalculate (G)
emissionIntereneProb = EmissionProbCalculate (I)
transitionProb = TransitionProbCalculate (geneLength, geneCount,
                                          intergeneLength, intergeneCount)
emissions = PseudoValueInsert ("s")
emissionS = PseudoValueInsert ("S")
emissione = PseudoValueInsert ("e")
emissionE = PseudoValueInsert ("E")

fmodel = open ("model.txt", 'w')

fmodel.write ("Transition=\n")
PrintFile (fmodel, transitionProb)

fmodel.write ("\nEmission_gene=\n")
PrintFile (fmodel, emissionGeneProb)

fmodel.write ("\nEmission_intergene=\n")
PrintFile (fmodel, emissionIntereneProb)

fmodel.write ("\nEmission_s=\n")
PrintFile (fmodel, emissions)

fmodel.write ("\nEmission_S=\n")
PrintFile (fmodel, emissionS)

fmodel.write ("\nEmission_e=\n")
PrintFile (fmodel, emissione)

fmodel.write ("\nEmission_E=\n")
PrintFile (fmodel, emissionE)
