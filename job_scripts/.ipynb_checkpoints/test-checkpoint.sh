D=1.9659
T=0.6079

pairs=(
  "3 3"
  "3.5 3"
  "2.5 3"
  "3 3.5"
  "3 2.5"
)

for pair in "${pairs[@]}"; do
    read ti di <<< "$pair"
	d=$(bc -l <<< "$D + 0.000025*$di")
	t=$(bc -l <<< "$T + 0.000025*$ti")
	echo $t $d
done




