def crunchy(n):
    if n < 2: return n
    return 6 * crunchy(n - 1) + crunchy(n - 2)

g = 17665922529512695488143524113273224470194093921285273353477875204196603230641896039854934719468650093602325707751568

print("Your flag is: INSA{%d}"%(crunchy(g)%100000007))
