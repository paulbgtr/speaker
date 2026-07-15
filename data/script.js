let stations = [];
let editingId = null;

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

const editStation = (id) => {
  const station = stations.find((s) => s.id === id);
  if (!station) return;

  editingId = id;
  addStationForm.elements["name"].value = station.name;
  addStationForm.elements["url"].value = station.url;

  document.getElementById("submit-btn").textContent = "Update station";
  document.getElementById("cancel-btn").style.display = "inline-block";
};

const exitEditMode = () => {
  editingId = null;
  addStationForm.reset();
  document.getElementById("submit-btn").textContent = "Add station";
  document.getElementById("cancel-btn").style.display = "none";
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
        <button class="edit-btn" title="Edit" onclick="editStation(${s.id})">
            edit
        </button>
        <button class="remove-btn" title="Remove" onclick="removeStation(${s.id})">
            &times;
        </button>
    </li>
    `;
    })
    .join("");
};

const addStationForm = document.querySelector("form");

addStationForm.addEventListener("submit", async (e) => {
  e.preventDefault();

  const formData = new FormData(e.target);
  const data = Object.fromEntries(formData.entries());

  if (editingId !== null) {
    await fetch("http://lofi-speaker.local/stations" + `?id=${editingId}`, {
      method: "PATCH",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(data),
    });
    exitEditMode();
  } else {
    await fetch("http://lofi-speaker.local/stations", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(data),
    });
    e.target.reset();
  }

  fetchStations();
});

document.getElementById("cancel-btn").addEventListener("click", exitEditMode);

fetchStations();
