

window.addEventListener('load', () => {
    once(), ost(), west(), general();
    let button = document.querySelectorAll('button');
    let span = document.querySelectorAll('#right span');

    button[1].addEventListener('click', () => {
        if (confirm('Dies verbindet in 5 Sekunden das Gerät neu! Bist du sicher!'));
        {
            re('reconnect');
        }
    });

    button[2].addEventListener('click', () => {
        if (confirm('Dies startet in 5 Sekunden das Gerät neu! Bist du sicher!'));
        {
            re('restart');
        }
    });


    function once() {
        myOst = setInterval(ost, 5000);
        myWest = setInterval(west, 5000);
        myGeneral = setInterval(general, 5000);
    }

    function ost() {
        let MaxOstEin = document.getElementById("Maximal Einschaltschwelle Ost");
        let MaxOstAus = document.getElementById("Maximal Ausschaltschwelle Ost");
        let MinOstEin = document.getElementById("Minimal Einschaltschwelle Ost");
        let MinOstAus = document.getElementById("Minimal Ausschaltschwelle Ost");
        let DiffOstEin = document.getElementById("Differenz Einschaltschwelle Ost");
        let DiffOstAus = document.getElementById("Differenz Ausschaltschwelle Ost");
        let OverOstEin = document.getElementById("Übertemperatur Einschaltschwelle Ost");
        let OverOstAus = document.getElementById("Übertemperatur Ausschaltschwelle Ost");
        let FrostOstEin = document.getElementById("Frostschutz Einschaltschwelle Ost");
        let FrostOstAus = document.getElementById("Frostschutz Ausschaltschwelle Ost");
        let CoolOstEin = document.getElementById("Kühlfunktion Einschaltschwelle Ost");
        let CoolOstFrei = document.getElementById("Kühlfunktion Freigabezeit Ost");
        let CoolOstSperr = document.getElementById("Kühlfunktion Sperrzeit Ost");
        fetch('parameter/ost').then((resp) => {
            return resp.json();
        }).then((array) => {
            array.forEach((x, i) => {
                MaxOstEin.innerHTML = array[0]
                MaxOstAus.innerHTML = array[1]
                MinOstEin.innerHTML = array[2]
                MinOstAus.innerHTML = array[3]
                DiffOstEin.innerHTML = array[4]
                DiffOstAus.innerHTML = array[5]
                OverOstEin.innerHTML = array[6]
                OverOstAus.innerHTML = array[7]
                FrostOstEin.innerHTML = array[8]
                FrostOstAus.innerHTML = array[9]
                CoolOstEin.innerHTML = array[10]
                CoolOstFrei.innerHTML = array[11]
                CoolOstSperr.innerHTML = array[12]
            });
        });
    }

    function west() {
        let MaxWestEin = document.getElementById("Maximal Einschaltschwelle West");
        let MaxWestAus = document.getElementById("Maximal Ausschaltschwelle West");
        let MinWestEin = document.getElementById("Minimal Einschaltschwelle West");
        let MinWestAus = document.getElementById("Minimal Ausschaltschwelle West");
        let DiffWestEin = document.getElementById("Differenz Einschaltschwelle West");
        let DiffWestAus = document.getElementById("Differenz Ausschaltschwelle West");
        let OverWestEin = document.getElementById("Übertemperatur Einschaltschwelle West");
        let OverWestAus = document.getElementById("Übertemperatur Ausschaltschwelle West");
        let FrostWestEin = document.getElementById("Frostschutz Einschaltschwelle West");
        let FrostWestAus = document.getElementById("Frostschutz Ausschaltschwelle West");
        let CoolWestEin = document.getElementById("Kühlfunktion Einschaltschwelle West");
        let CoolWestFrei = document.getElementById("Kühlfunktion Freigabezeit West");
        let CoolWestSperr = document.getElementById("Kühlfunktion Sperrzeit West");
        fetch('parameter/west').then((resp) => {
            return resp.json();
        }).then((array) => {
            array.forEach((x, i) => {
                MaxWestEin.innerHTML = array[0]
                MaxWestAus.innerHTML = array[1]
                MinWestEin.innerHTML = array[2]
                MinWestAus.innerHTML = array[3]
                DiffWestEin.innerHTML = array[4]
                DiffWestAus.innerHTML = array[5]
                OverWestEin.innerHTML = array[6]
                OverWestAus.innerHTML = array[7]
                FrostWestEin.innerHTML = array[8]
                FrostWestAus.innerHTML = array[9]
                CoolWestEin.innerHTML = array[10]
                CoolWestFrei.innerHTML = array[11]
                CoolWestSperr.innerHTML = array[12]
            });
        });
    }

    function general() {
        let VentilEin = document.getElementById("Umschaltventil Einschaltschwelle");
        let VentilAus = document.getElementById("Umschaltventil Ausschaltschwelle");
        let VentilFreigabe = document.getElementById("Umschaltventil Freigabetemperatur");
        let UmladepumpeEin = document.getElementById("Umladepumpe Einschaltschwelle");
        let UmladepumpeAus = document.getElementById("Umladepumpe Ausschaltschwelle");
        let UmladepumpeFreigabe = document.getElementById("Umladepumpe Freigabetemperatur");
        let UmladepumpeSPoben = document.getElementById("Umladepumpe Sensor Schaltpunkt oben");
        let UmladepumpeSPunten = document.getElementById("Umladepumpe Sensor Schaltpunkt unten");
        fetch('parameter/general').then((resp) => {
            return resp.json();
        }).then((array) => {
            array.forEach((x, i) => {
                VentilEin.innerHTML = array[0]
                VentilAus.innerHTML = array[1]
                VentilFreigabe.innerHTML = array[2]
                UmladepumpeEin.innerHTML = array[3]
                UmladepumpeAus.innerHTML = array[4]
                UmladepumpeFreigabe.innerHTML = array[5]

                switch (array[6]) {
                    case "0":
                        UmladepumpeSPoben.innerHTML = "Warmwasser";
                        break;

                    case "1":
                        UmladepumpeSPoben.innerHTML = "Puffer Mitte";
                        break;

                    case "2":
                        UmladepumpeSPoben.innerHTML = "Puffer Unten";
                        break;

                    case "3":
                        UmladepumpeSPoben.innerHTML = "Vorlauf";
                        break;

                    case "4":
                        UmladepumpeSPoben.innerHTML = "Rücklauf";
                        break;
                }

                switch (array[7]) {
                    case "0":
                        UmladepumpeSPunten.innerHTML = "Warmwasser";
                        break;

                    case "1":
                        UmladepumpeSPunten.innerHTML = "Puffer Mitte";
                        break;

                    case "2":
                        UmladepumpeSPunten.innerHTML = "Puffer Unten";
                        break;

                    case "3":
                        UmladepumpeSPunten.innerHTML = "Vorlauf";
                        break;

                    case "4":
                        UmladepumpeSPunten.innerHTML = "Rücklauf";
                        break;
                }
            });
        });
    }

    function re(arg) {
        clearInterval(myOst);
        clearInterval(myWest);
        clearInterval(myGeneral);
        fetch(arg);
        if (arg == 'restart') {
            console.log("Restart");
            setTimeout(once, 5000);
        }
        else if (arg == 'reconnect') {
            setTimeout(once, 5000);
        }
        else {
            setTimeout(once, 5000);
        }
    }
});

