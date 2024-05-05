(fn int main ((int x) (int y)) () (
    begin
        (fn int add ((int x) (int y)) (
            + x y
        ))
        (add x y)
))