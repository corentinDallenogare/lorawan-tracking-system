const express = require("express")
const fs = require("fs")

const app = express()

app.use(express.json())

let data = JSON.parse(fs.readFileSync("./data.json"))

// API

app.post("/api/input", function(req, res) {
    const json = req.body
    console.log(req.body)
    data.concat(json)
    fs.writeFileSync("./data.json", JSON.stringify(data))
    res.send("yeet")
})

app.get("/api/input", (req, res) => res.json(data))

// FORONTEND

app.use(express.static('public'))

app.listen(process.env.PORT || 5000)
