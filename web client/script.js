document.addEventListener('DOMContentLoaded', (event) => {
  const dogFeedButton = document.getElementById('feedDogButton');
  const dogFeedAmount = document.getElementById('dogFeedAmount');
  const catFeedButton = document.getElementById('feedCatButton');
  const catFeedAmount = document.getElementById('catFeedAmount');
  const logEntries = document.getElementById('logEntries');
  const connectionStatus = document.getElementById('connectionStatus');
  const defaultAmountInput = document.getElementById('defaultAmount');
  const setDefaultButton = document.getElementById('setDefaultButton');

  const catBowlProgressBar = document.getElementById("catBowlProgressBar");
  const dogBowlProgressBar = document.getElementById("dogBowlProgressBar");
  const bowlStorageProgressBar = document.getElementById("bowlStorageProgressBar");

  const catBowlStatusPercentage = document.getElementById("catBowlStatusPercentage");
  const dogBowlStatusPercentage = document.getElementById("dogBowlStatusPercentage");
  const bowlStorageStatusPercentage = document.getElementById("bowlStorageStatusPercentage");





  const ws = new WebSocket('ws://192.168.23.67:8888/ws');
  const refreshInterval = 5000; 



  

  

  function checkStatusAutomatically() {
    ws.send(JSON.stringify({ action: 'checkStatus' }));
  }


  function updateProgressBarWidth(progressBarElement, fillLevel) {
    progressBarElement.style.width = `${100-(fillLevel/50) }%`;
  }

  


  

  
 

  ws.onopen = function () {
    connectionStatus.textContent = 'Connected';
  };

  ws.onclose = function () {
    connectionStatus.textContent = 'Disconnected';
  };


  ws.onmessage = function (event) {
    const data = JSON.parse(event.data);
  
    if (data.pet_type && data.is_bowl_full) {
      const newRow = document.createElement('tr');
  
      const detectionTimeCell = document.createElement('td');
      detectionTimeCell.textContent = data.detection_time;
      newRow.appendChild(detectionTimeCell);
  
      const petTypeCell = document.createElement('td');
      petTypeCell.textContent = data.pet_type;
      newRow.appendChild(petTypeCell);
  
      const isBowlFullCell = document.createElement('td');
      isBowlFullCell.textContent = data.is_bowl_full ? 'Yes' : 'No';
      newRow.appendChild(isBowlFullCell);
  
      logEntries.appendChild(newRow);
    }
  };



  setDefaultButton.addEventListener('click', function () {
    const defaultAmount = defaultAmountInput.value;
    if (defaultAmount) {
      ws.send(defaultAmount); 
      defaultAmountInput.value = '';
    } else {
      alert('Please enter the default amount of food.');
    }
  });
  
  dogFeedButton.addEventListener('click', function () {
    const amount = dogFeedAmount.value;
    if (amount) {
      ws.send(amount); 
      dogFeedAmount.value = '';
    } else {
      alert('Please enter the amount of dog food.');
    }
  });
  
  catFeedButton.addEventListener('click', function () {
    const amount = catFeedAmount.value;
    if (amount) {
      ws.send(amount); 
      catFeedAmount.value = '';
    } else {
      alert('Please enter the amount of cat food.');
    }
  });
  



  if (ws.readyState === WebSocket.OPEN) {
    const receivedBowlStatusData = ws.receive().data.split(',');

    if (receivedBowlStatusData.length === 3) {
      const dogBowlFillLevel = parseFloat(receivedBowlStatusData[0]);
      const catBowlFillLevel = parseFloat(receivedBowlStatusData[1]);
      const BowlFillLevel = parseFloat(receivedBowlStatusData[2]);
 
      updateProgressBarWidth(catBowlProgressBar, catBowlFillLevel);
      updateProgressBarWidth(dogBowlProgressBar, dogBowlFillLevel);
      updateProgressBarWidth(bowlStorageProgressBar, BowlFillLevel); 

      catBowlStatusPercentage.textContent = `${100-(catBowlFillLevel/50)}%`;
      dogBowlStatusPercentage.textContent = `${100-(dogBowlFillLevel/50)}%`;
      bowlStorageStatusPercentage.textContent = `${100-(bowlStorageFillLevel/50)}%`;
  
    }
  }



});
