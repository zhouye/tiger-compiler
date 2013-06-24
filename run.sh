for i in {1..49}
do
	echo Test Case $i
	cat test/test$i.tig | grep error
	./tigerc test/test$i.tig test/test$i.ll
done
