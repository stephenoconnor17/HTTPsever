const btn = document.getElementById("btn");
const out = document.getElementById("out");

if (btn) {
  let count = 0;
  btn.addEventListener("click", () => {
    count++;
    out.textContent = "Clicked " + count + " time(s)";
  });
}
const infoOut = document.getElementById("info");

if (infoOut) {
  fetch("/info.json")
    .then(r => r.json())
    .then(people => {
      infoOut.textContent = people
        .map(p => p.name + " (" + p.age + "): " + p.languages.join(", "))
        .join(" | ");
    })
    .catch(err => console.error("fetch failed:", err));
}