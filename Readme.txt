1. Build the tool
   cd TrafficCap && make clean && make
   
2. Run the tool
   2.1 Start TrafficCap
       cd TrafficCap
	   ./TfCapture -d "network adapter"
   2.2 Start the self-learning system
       cd Classifier
	   ./Run.py
	   
3. Protocol identification and features extraction
   Protocol Types: Classifier/data/labelctrl.csv
   Protocol Features: Classifier/result/Appcharacteristic.csv