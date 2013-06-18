for i in {1..49}
do
	echo Test Case $i
	cat test/test$i.tig | grep error
	cat test/test$i.tig | ./tigerc
done
