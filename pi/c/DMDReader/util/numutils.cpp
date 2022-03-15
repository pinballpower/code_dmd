int roundup_4(int numToRound)
{
    int remainder = numToRound % 4;
    if (remainder == 0)
        return numToRound;

    return numToRound + 4 - remainder;
}