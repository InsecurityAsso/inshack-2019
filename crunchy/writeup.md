Turn the series into a matrix equation, compute with fast modular exponentiation algorithm.

```python
def matrix_multiply(mat_a, mat_b, n):
    a, b, c, d = mat_a
    x, y, z, w = mat_b

    return (
        (a * x + b * z) % n,
        (a * y + b * w) % n,
        (c * x + d * z) % n,
        (c * y + d * w) % n,
    )

def matrix_power(A, m, mod):
    if m == 0:
        return [1, 0, 0, 1]
    elif m == 1:
        return A
    else:
        B = A
        n = 2
        while n <= m:
            B = matrix_multiply(B, B, mod)
            n = n*2
        R = matrix_power(A, m-n//2, mod)
        return matrix_multiply(B, R, mod)

F1 = [6, 1,
      1, 0]

g = 17665922529512695488143524113273224470194093921285273353477875204196603230641896039854934719468650093602325707751568

print("INSA{%d}"%matrix_power(F1, g, 100000007)[1])


```