let stations = [];

const fetchStations = async () => {
  const res = await fetch("http://lofi-speaker.local/stations");

  stations = await res.json();
  render();
};

const removeStation = async (id) => {
  await fetch("http://lofi-speaker.local/stations" + `?id=${id}`, {
    method: "DELETE",
  });

  fetchStations();
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
        <button class="remove-btn" title="Remove" onclick=removeStation(${s.id})>
            &times;
        </button>
    </li>
    `;
    })
    .join("");
};

fetchStations();
