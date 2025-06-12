fsys=125000000
target=6666
best_match = 0
best_diff = 1
for x in range(125000000):
    if x%1000000 == 0:
        print(f"Checking {x}/125000000")
    if x%3!=0:
        continue
    clkdiv = fsys / (target+x)
    if clkdiv > 256:
        continue
    d = round(clkdiv) - clkdiv 
    if d<best_diff:
        best_diff = d
        best_match = x
print(f"Best match {best_match}, diff {best_diff}")
