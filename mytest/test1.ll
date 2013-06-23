@0 = private unnamed_addr constant [4 x i8] c"%d \00"
@1 = private unnamed_addr constant [3 x i8] c"%d\00"

define internal i32 @main() {
entry:
  %a = alloca i32
  store i32 10, i32* %a
  %b = alloca i32
  store i32 10, i32* %b
  %0 = load i32* %a
  %1 = icmp eq i32 %0, 10
  br i1 %1, label %then, label %ifcont

then:                                             ; preds = %entry
  %2 = load i32* %b
  store i32 0, i32* %b
  br label %ifcont

ifcont:                                           ; preds = %then, %entry
  br label %cond

cond:                                             ; preds = %loop, %ifcont
  %3 = load i32* %a
  %4 = icmp sgt i32 %3, 0
  br i1 %4, label %loop, label %merge

loop:                                             ; preds = %cond
  %5 = load i32* %b
  %6 = load i32* %b
  %7 = load i32* %a
  %8 = add i32 %6, %7
  store i32 %8, i32* %b
  %9 = load i32* %a
  %10 = load i32* %a
  %11 = call i32 @f(i32 %10)
  store i32 %11, i32* %a
  br label %cond

merge:                                            ; preds = %cond
  %12 = load i32* %a
  %13 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @0, i32 0, i32 0), i32 %12)
  %14 = load i32* %b
  %15 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([3 x i8]* @1, i32 0, i32 0), i32 %14)
  ret i32 %15
}

declare i32 @printf(i8*, ...)

define internal i32 @f(i32) {
entry:
  %c = alloca i32
  store i32 %0, i32* %c
  %1 = load i32* %c
  %2 = sub i32 %1, 1
  ret i32 %2
}
