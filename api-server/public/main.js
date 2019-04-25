"use strict";

document.addEventListener("DOMContentLoaded", init)

function init() {
    fetch("/api/input")
        .then(res => res.json())
        // .then(() => [
        //     {
        //         lat: 50.48,
        //         long: 4.6,
        //         alt: 50.48
        //     }, {
        //         lat: 50.79878,
        //         long: 4.8,
        //         alt: 50.48
        //     }, {
        //         lat: 50.498,
        //         long: 4.8098,
        //         alt: 50.48
        //     }
        // ])
        .then(showData)
}

function showData(arrayOfData) {
    const dataHtml = document.querySelector("#data table tbody")

    // TODO: make a table of data
    
    let tableHtml = ""
    for (const obj of arrayOfData)
        tableHtml += `
        <tr>
            <th>${obj.lat}</th>
            <th>${obj.long}</th>
            <th>${obj.alt}</th>
        </tr>
        `

    dataHtml.innerHTML += tableHtml

    // SHOW MAP

    const mymap = L.map('map').setView([50.48, 4.6], 10);

    L.tileLayer('http://{s}.tile.osm.org/{z}/{x}/{y}.png', {}).addTo(mymap)
    L.polyline(arrayOfData.map(obj => [obj.lat, obj.long])).addTo(mymap)

    const obj = arrayOfData[arrayOfData.length - 1]
    L.marker([obj.lat, obj.long]).addTo(mymap)
}
