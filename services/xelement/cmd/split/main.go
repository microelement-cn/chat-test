package main

import (
	"fmt"
)

func main() {
	fmt.Println("Split test begin")
	var a = [...]int{1, 2, 3}
	fmt.Println(a)
	for i, v := range a {
		fmt.Printf("index-value:%d-%d\n", i, v)
	}

	var s1 = [...]string{"Hello", "World", "LiKun"}
	fmt.Println(s1)
	fmt.Printf("s1 %T\n", s1)
	fmt.Printf("s1 %#v\n", s1)
	for i, v := range s1 {
		fmt.Printf("index-value:%d-%s\n", i, v)
	}

	c1 := make(chan [0]int)
	go func() {
		fmt.Println("c1 boot")
		c1 <- [0]int{}
	}()
	<-c1
	fmt.Println("c1 over")

	c2 := make(chan struct{})
	go func() {
		fmt.Println("c2 boot")
		c2 <- struct{}{}
	}()
	<-c2
	fmt.Println("c2 over")
}
