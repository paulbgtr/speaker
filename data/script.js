let stations = [];

const fetchStations = async () => {
  const res = await fetch("http://lofi-speaker.local/stations");

  stations = await res.json();
  console.log(stations);
  render();
};

const render = () => {
  const container = document.getElementById("stations");

  container.innerHTML = stations
    .map((s) => {
      return `
    <li>
        <div>
            <div class="station-name">${s.name}</div>
            <div class="station-url">
              ${s.url}
            </div>
        </div>
        <button class="remove-btn" title="Remove">
            &times;
        </button>
    </li>
    `;
    })
    .join("");
};

fetchStations();
