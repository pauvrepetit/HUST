inputMD5=$(md5sum input.txt)
inputMD5=${inputMD5% *}
outputMD5=$(md5sum output.txt)
outputMD5=${outputMD5% *}
md5sum input.txt
md5sum output.txt
if [ "$inputMD5" == "$outputMD5" ]
then
echo ok
else
echo not ok
fi
