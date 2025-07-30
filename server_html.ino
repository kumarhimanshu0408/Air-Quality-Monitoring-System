
const String html = R"rawliteral(<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Air Quality Index (AQI) Monitor</title>
    <style>
      html {
        top: 0;
        height: 100vh;
        overflow-y: hidden;
      }
      body {
        font-family: Arial, sans-serif;
        background: rgba(172, 233, 255, 0.663);
        color: #333;
        display: flex;
        align-items: center;
        flex-direction: column;
        margin: 0;
        text-align: center;
        box-sizing: border-box;
        height: 100vh;
        overflow: scroll;
      }
      .container {
        margin: auto;
        width: 100%;
        display: flex;
        flex-grow: 1;
        align-items: center;
        justify-content: center;
        flex-direction: column;
        padding: 20px 0;
        margin-top: 80px;
        text-align: center;
      }

      .aqi-card {
        background: white;
        padding: 20px;
        display: none;
        box-shadow: 0 6px 12px rgba(0, 0, 0, 0.3);
        font-size: 60px;
        font-weight: bold;
        border-radius: 10px;
        background-color: #b3ffb3;
        color: rgba(217, 236, 223, 0.963);
        margin: 20px auto;
        text-align: center;
        width: 180px;
        height: 180px;
        display: flex;
        align-items: center;
        justify-content: center;
      }

      .card-container {
        display: flex;
        width: max(60%, 200px);
        flex-wrap: wrap;
        justify-content: center;
        gap: 10px;
      }

      .card {
        color: black;
        padding: 10px;
        font-size: 20px;
      }

      .card-value div:nth-child(1) {
        font-size: 40px;
        color: #ffffff;
      }
      .card-value div:nth-child(2) {
        font-size: 18px;
        color: rgb(147, 142, 142);
      }

      /* Responsive: Stack cards on small screens */
      @media (max-width: 500px) {
        .card-container {
          flex-direction: column;
          align-items: center;
        }
      }

      h1 {
        color: #333;
      }
      .sensor-data {
        font-size: 20px;
        margin: 10px 0;
        font-weight: bold;
      }
      .aqi {
        font-size: 30px;
        font-weight: bold;
        padding: 10px;
        border-radius: 5px;
        color: white;
      }
      .good {
        background: #b3ffb3;
      }
      .moderate {
        background: #d6d629;
        color: black; /* Ensure text is readable */
      }
      .poor {
        background: #ffd1b3;
      }
      .unhealthy {
        background: #fea8a8;
      }
      .very-unhealthy {
        background: #d9b3d9;
      }
      .hazardous {
        background: #c64b4bdb;
      }

      .pm25-good {
        background: #b3ffb3; /* Lighter green */
      }

      .pm25-moderate {
        background: #d6d629; /* Lighter yellow */
      }

      .pm25-poor {
        background: #ffd1b3; /* Lighter orange */
      }

      .pm25-unhealthy {
        background: #ffb3b3; /* Lighter red */
      }

      .pm25-hazardous {
        background: #c64b4bdb; /* Lighter maroon */
      }

      /* Similar styles for PM10 and CO */
      .pm10-good,
      .co-good {
        background: #b3ffb3;
      }

      .pm10-moderate,
      .co-moderate {
        background: #d6d629;
      }

      .pm10-poor,
      .co-poor {
        background: #ffd1b3;
      }

      .pm10-unhealthy,
      .co-unhealthy {
        background: #fea8a8;
      }

      .pm25-very-unhealthy,
      .pm10-very-unhealthy,
      .co-very-unhealthy {
        background: #d9b3d9;
      }

      .pm10-hazardous,
      .co-hazardous {
        background: #c64b4bdb;
      }

      .aqi-card,
      .card {
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        position: relative;
      }

      .card-value {
        font-size: 30px;
        font-weight: bold;
        margin-bottom: 10px;
      }

      .card-label {
        position: absolute;
        bottom: 8px;
        font-size: 16px;
        font-weight: bold;
        color: black;
      }

      .header {
        position: fixed;
        top: 0;
        width: 100%;
        background: rgba(255, 255, 255, 0.8);
        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
        padding: 10px 20px;
        display: flex;
        box-sizing: border-box;
        left: 0;
        height: 80px;
        justify-content: space-between;
        align-items: center;
        z-index: 1000;
      }

      .header-title {
        font-size: 20px;
        font-weight: bold;
        color: #333;
      }

      .header-right {
        font-size: 14px;
        color: #555;
      }

      @property --myColor1 {
        syntax: "#444343";
        initial-value: #444343;
        inherits: false;
      }

      @property --myColor2 {
        syntax: "black";
        initial-value: black;
        inherits: false;
      }

      @property --myColor3 {
        syntax: "transparent";
        initial-value: transparent;
        inherits: false;
      }

      .progress-bar {
        position: absolute;
        width: 150px;
        height: 150px;
        border-radius: 50%;
        background: conic-gradient(
          #b3ffb3 0%,
          #d6d629 10%,
          #ffd1b3 20%,
          #ff8c8c 40%,
          #b373b3 70%,
          #7b7b7b 100%
        );
        mask: radial-gradient(circle, transparent 55%, white 56%);
        -webkit-mask: radial-gradient(circle, transparent 55%, white 56%);
        transition: all 1s ease;
        transition: all 1s ease; /* Smooth transition for background */
      }

      .progress-bar-circle {
        position: absolute;
        width: 20px;
        height: 20px;
        background: black;
        border-radius: 50%;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%) rotate(0deg) translateY(-68px);
        transition: transform 1s steps(20);
      }

      .progress-bar-value {
        position: absolute;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
        font-size: 40px;
        font-weight: bold;
        color: #ffffff;
      }

      .legend-table {
        width: 90%;
        max-width: 500px;
        margin: 20px auto;
        border-collapse: collapse;
        font-size: 14px;
        text-align: left;
      }

      .legend-table th,
      .legend-table td {
        padding: 10px;
        border: 1px solid #ddd;
      }

      .legend-table th {
        background-color: #f4f4f4;
        font-weight: bold;
      }

      .legend-color {
        width: 20px;
        height: 20px;
        display: inline-block;
        border-radius: 3px;
      }

      /* Responsive adjustments */
      @media (max-width: 500px) {
        .legend-table {
          font-size: 12px;
        }
      }

      .scroll-button {
        background-color: #007bff;
        color: white;
        border: none;
        padding: 8px 12px;
        border-radius: 5px;
        cursor: pointer;
        font-size: 14px;
      }

      .scroll-button:hover {
        background-color: #0056b3;
      }
    </style>
  </head>
  <body>
    <div class="header">
      <div class="header-title">Air Quality Monitor</div>
      <div class="header-right">
        <span>IIT Palakkad</span> | <span id="current-date"></span>
        <button class="scroll-button" onclick="scrollToTable()">
          View Legend
        </button>
      </div>
    </div>
    <div class="container">
      <div id="aqi-card" class="aqi-card" style="display:none">
        <div class="progress-bar"></div>
        <div class="progress-bar" id="mask"></div>
        <div class="progress-bar-circle" id="progress-circle"></div>
        <div class="progress-bar-value">--</div>
        <div class="card-label">AQI</div>
      </div>
      <div class="card-container">
        <div class="aqi-card card" id="pm25_card">
          <div class="card-value">
            <div id="pm25">--</div>
            <div>mg/m3</div>
          </div>
          <div class="card-label">PM2.5</div>
        </div>
        <div class="aqi-card card" id="pm10_card">
          <div class="card-value">
            <div id="pm10">--</div>
            <div>mg/m3</div>
          </div>
          <div class="card-label">PM10</div>
        </div>
        <div class="aqi-card card" id="co_card">
          <div class="card-value">
            <div id="co">--</div>
            <div>mg/m3</div>
          </div>
          <div class="card-label">CO</div>
        </div>
      </div>

      <table class="legend-table" id="legend-table">
        <thead>
          <tr>
            <th>Color</th>
            <th>Category</th>
            <th>Description</th>
          </tr>
        </thead>
        <tbody>
          <tr>
            <td>
              <span class="legend-color" style="background: #b3ffb3"></span>
            </td>
            <td>Good</td>
            <td>Air quality is considered satisfactory.</td>
          </tr>
          <tr>
            <td>
              <span class="legend-color" style="background: #d6d629"></span>
            </td>
            <td>Moderate</td>
            <td>
              Air quality is acceptable; some pollutants may affect sensitive
              groups.
            </td>
          </tr>
          <tr>
            <td>
              <span class="legend-color" style="background: #ffd1b3"></span>
            </td>
            <td>Poor</td>
            <td>Members of sensitive groups may experience health effects.</td>
          </tr>
          <tr>
            <td>
              <span class="legend-color" style="background: #fea8a8"></span>
            </td>
            <td>Unhealthy</td>
            <td>Everyone may begin to experience health effects.</td>
          </tr>
          <tr>
            <td>
              <span class="legend-color" style="background: #d9b3d9"></span>
            </td>
            <td>Very Unhealthy</td>
            <td>Health alert: everyone may experience serious effects.</td>
          </tr>
          <tr>
            <td>
              <span class="legend-color" style="background: #c64b4bdb"></span>
            </td>
            <td>Hazardous</td>
            <td>Health warnings of emergency conditions.</td>
          </tr>
        </tbody>
      </table>
    </div>

    <script>
      // Set the current date in the header
      document.getElementById("current-date").innerText =
        new Date().toLocaleDateString();

      // Function to scroll to the legend table
      function scrollToTable() {
        document.getElementById("legend-table").scrollIntoView({
          behavior: "smooth",
          block: "start",
        });
      }

      /* const val = [];
      let i = 0;
      for (let k = 0; k < 500; k += 40) {
        console.log(k);
        val.push(k);
        val.push(k);
        val.push(k);
        val.push(k);
      } */
      let currentPercent = 0;
      async function fetchData() {
        try {
          const response = await fetch("/read");
          const data = await response.json();
          /* const data = {
            "PM2.5": Math.random() * 100,
            PM10: Math.random() * 100,
            CO: Math.random() * 100,
          }; // testing */
          const pm25 = data["PM2.5"].toFixed(2);
          const pm10 = data["PM10"].toFixed(2);
          const co = data["CO"].toFixed(2);

          document.querySelector("#pm25").innerText = pm25;
          document.querySelector("#pm10").innerText = pm10;
          document.querySelector("#co").innerText = co;

          document.getElementById("pm25_card").className =
            "aqi-card card " + "pm25-" + get_AQI_category(get_AQI(pm25, 0, 0));
          document.getElementById("pm10_card").className =
            "aqi-card card " + "pm10-" + get_AQI_category(get_AQI(0, pm10, 0));
          document.getElementById("co_card").className =
            "aqi-card card " + "co-" + get_AQI_category(get_AQI(0, 0, co));

          let AQI = get_AQI(data["PM2.5"], data["PM10"], data["CO"]);
          let aqiCard = document.getElementById("aqi-card");
          document.querySelector("#aqi-card .progress-bar-value").innerText =
            AQI;
          aqiCard.className = "aqi-card " + get_AQI_category(AQI);

          // Update progress bar percentage smoothly
          const progressBar = document.getElementById("mask");
          const targetPercent = (AQI / 500.0) * 100;

          const step = (targetPercent - currentPercent) / 20; // 20 steps for 1 second
          const interval = setInterval(() => {
            currentPercent += step;
            progressBar.style.background = `conic-gradient(#0f0f0f54 0%, black ${currentPercent}%, transparent ${currentPercent}%)`;
          }, 50); // 50ms per step

          setTimeout(() => {
            clearInterval(interval);
            currentPercent = targetPercent;
            progressBar.style.background = `conic-gradient(#0f0f0f54 0%, black ${targetPercent}%, transparent ${targetPercent}%)`;
          }, 1000); // Forcefully set after 1 second

          // Update progress bar circle rotation
          const progressBarCircle = document.getElementById("progress-circle");
          const rotation = (AQI / 500) * 360;
          progressBarCircle.style.transform = `translate(-50%, -50%) rotate(${rotation}deg) translateY(-66.5px)`;
        } catch (error) {
          console.error("Error fetching data:", error);
          document.querySelector("#aqi-card .progress-bar-value").innerText =
            "Error";
        }
      }

      function get_AQI(pm25, pm10, co) {
        let sub_PM25 = calculate_subindex(
          pm25,
          [0, 30, 60, 90, 120, 250, 500],
          [0, 50, 100, 200, 300, 400, 500]
        );
        let sub_PM10 = calculate_subindex(
          pm10,
          [0, 50, 100, 250, 350, 430, 600],
          [0, 50, 100, 200, 300, 400, 500]
        );
        let sub_CO = calculate_subindex(
          co,
          [0, 1, 2, 10, 17, 34, 50],
          [0, 50, 100, 200, 300, 400, 500]
        );

        return Math.max(sub_PM25, sub_PM10, sub_CO);
      }

      function calculate_subindex(concentration, breakpoints, aqi_values) {
        for (let i = 0; i < breakpoints.length - 1; i++) {
          if (
            concentration >= breakpoints[i] &&
            concentration <= breakpoints[i + 1]
          ) {
            return Math.round(
              ((aqi_values[i + 1] - aqi_values[i]) /
                (breakpoints[i + 1] - breakpoints[i])) *
                (concentration - breakpoints[i]) +
                aqi_values[i]
            );
          }
        }
        return 500; // Max AQI for out-of-range values
      }

      function get_AQI_category(aqi) {
        if (aqi <= 50) return "good";
        if (aqi <= 100) return "moderate";
        if (aqi <= 200) return "poor";
        if (aqi <= 300) return "unhealthy";
        if (aqi <= 400) return "very-unhealthy";
        return "hazardous";
      }

      fetchData();
      setInterval(fetchData, 5000); // Refresh every 5 seconds
    </script>
  </body>
</html>
)rawliteral";

const String change_mode_html = R"rawliteral(
  <!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>WiFi Credentials</title>
    <style>
      body {
        font-family: Arial, sans-serif;
        text-align: center;
        margin: 0;
        padding: 0;
        display: flex;
        justify-content: center;
        align-items: center;
        height: 100vh;
        background-color: #f4f4f4;
      }
      * {
        box-sizing: border-box;
      }
      .container {
        width: 300px;
        padding: 20px;
        background: white;
        box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.2);
        border-radius: 10px;
      }
      .credentials-box {
        margin-bottom: 20px;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        text-align: left;
      }
      .credentials-box p {
        margin: 5px 0;
      }
      input {
        width: 100%;
        padding: 10px;
        margin-top: 10px;
        border: 1px solid #ccc;
        border-radius: 5px;
      }
      button {
        width: 100%;
        padding: 10px;
        background-color: #007bff;
        color: white;
        border: none;
        border-radius: 5px;
        cursor: pointer;
        margin-top: 10px;
      }
      button:hover {
        background-color: #0056b3;
      }
      .loading-screen {
        position: fixed;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        background-color: rgba(0, 0, 0, 0.8);
        display: flex;
        justify-content: center;
        align-items: center;
        color: white;
        font-size: 20px;
        z-index: 1000;
        display: none;
      }
    </style>
  </head>
  <body>
    <div class="container">
      <div class="credentials-box">
        <p><strong>Current SSID:</strong> <div id="currentSSID">--</div></p>
        <p>
          <strong>Current Password:</strong>
          <div id="currentPassword">--</div>
        </p>
        <p>
          <strong>IP Address:</strong>
          <div id="ip">--</div>
        </p>
      </div>
      <h2>Change WiFi Credentials</h2>
      <input type="text" id="newSSID" placeholder="Enter New SSID" />
      <input
        type="password"
        id="newPassword"
        placeholder="Enter New Password"
      />
      <input
        type="password"
        id="adminPassword"
        placeholder="Enter Admin Password"
      />
      <button onclick="submitNewCredentials()">Submit</button>
    </div>
    <div id="loadingScreen" class="loading-screen">Loading...</div>

    <script>
      let currentSSID = "##SSID##"; // Placeholder for current SSID
      let currentPassword = "##PASSWORD##"; // Placeholder for current password
      let currentIP = "##IP##"; // Placeholder for current IP address

      function updateCurrentCredentials() {
        document.getElementById("currentSSID").innerText = currentSSID;
        document.getElementById("currentPassword").innerText = currentPassword;
        document.getElementById("ip").innerText = currentIP;
      }

      function showLoadingScreen() {
        document.getElementById("loadingScreen").style.display = "flex";
      }

      function hideLoadingScreen() {
        document.getElementById("loadingScreen").style.display = "none";
      }

      function submitNewCredentials() {
        let newSSID = document.getElementById("newSSID").value;
        let newPassword = document.getElementById("newPassword").value;
        let adminPassword = document.getElementById("adminPassword").value;

        if (!newSSID || !newPassword || !adminPassword) {
          alert("Please enter both SSID and password.");
          return;
        }

        showLoadingScreen();

        fetch("/admin/change_creds", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ ssid: newSSID, password: newPassword, admin: adminPassword }),
        })
          .then((response) => response.json())
          .then((res) => {
            if (res.success) {
              currentSSID = res.ssid;
              currentPassword = res.password;
              currentIP = res.ip;
              // Update the displayed credentials
              updateCurrentCredentials();
              alert("Credentials updated successfully!");
            } else {
              alert("Error: " + res.message);
            }
          })
          .catch((error) => console.error("Error:", error))
          .finally(() => hideLoadingScreen());
      }

      // Initialize the UI with current credentials
      updateCurrentCredentials();
    </script>
  </body>
</html>
)rawliteral";