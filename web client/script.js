document.addEventListener('DOMContentLoaded', (event) => {
  const dogFeedButton = document.getElementById('feedDogButton');
  const dogFeedAmount = document.getElementById('dogFeedAmount');
  const catFeedButton = document.getElementById('feedCatButton');
  const catFeedAmount = document.getElementById('catFeedAmount');
  const logEntries = document.getElementById('logEntries');
  const connectionStatus = document.getElementById('connectionStatus');
  const defaultAmountInput = document.getElementById('defaultAmount');
  const setDefaultButton = document.getElementById('setDefaultButton');
  const dogBowlStatusSpan = document.getElementById('dogBowlStatus');
  const catBowlStatusSpan = document.getElementById('catBowlStatus');

  const ws = new WebSocket('ws://192.168.X.XXX:8888/ws');
  const refreshInterval = 5000; // Postavite interval prema potrebi, npr. svakih 5000 milisekundi (5 sekundi)

  function checkStatusAutomatically() {
    ws.send(JSON.stringify({ action: 'checkStatus' }));
  }

  // Pozovi funkciju checkStatusAutomatically svakih refreshInterval milisekundi
  const refreshIntervalId = setInterval(checkStatusAutomatically, refreshInterval);

  // Event listener za zatvaranje stranice, kako bismo prekinuli interval kada korisnik napusti stranicu
  window.addEventListener('beforeunload', function () {
    clearInterval(refreshIntervalId);
  });

  ws.onopen = function () {
    connectionStatus.textContent = 'Connected';
  };

  ws.onclose = function () {
    connectionStatus.textContent = 'Disconnected';
  };

  ws.onmessage = function (event) {
    const data = JSON.parse(event.data);

    if (data.action === 'statusUpdate') {
      dogBowlStatusSpan.textContent = `Level: ${data.dogBowlStatus} g`;
      catBowlStatusSpan.textContent = `Level: ${data.catBowlStatus} g`;
    } else {
      const message = document.createElement('div');
      message.textContent = event.data;
      logEntries.appendChild(message);
    }
  };

  setDefaultButton.addEventListener('click', function () {
    const defaultAmount = defaultAmountInput.value;
    if (defaultAmount) {
      ws.send(JSON.stringify({ action: 'setDefault', amount: defaultAmount }));
      defaultAmountInput.value = '';
    } else {
      alert('Please enter the default amount of food.');
    }
  });

  dogFeedButton.addEventListener('click', function () {
    const amount = dogFeedAmount.value;
    if (amount) {
      ws.send(JSON.stringify({ action: 'feedDog', amount }));
      dogFeedAmount.value = '';
    } else {
      alert('Please enter the amount of dog food.');
    }
  });

  catFeedButton.addEventListener('click', function () {
    const amount = catFeedAmount.value;
    if (amount) {
      ws.send(JSON.stringify({ action: 'feedCat', amount }));
      catFeedAmount.value = '';
    } else {
      alert('Please enter the amount of cat food.');
    }
  });
});

  
