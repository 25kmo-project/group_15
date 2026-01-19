//haetaan tietokantayhteys database.js-tiedostosta
const db = require('../database');

//luodaan objektin nimeltä user
const user={
    
//saadaan kaikki tiedot taulusta user
    getAll: function(callback){
        return db. query ("SELECT * FROM user", callback);
    },
//saadaan yhden user:n tiedot (käytetään user_id tässä kun se on pääavain)
    getOne: function (user_id, callback){
        return db. query ("SELECT * FROM user WHERE user_id=?", [user_id], callback);
    },
 //lisätään uusi user   
    add: function (user, callback) {
        return db. query ("INSERT INTO user VALUES (?,?,?,?,?)", [user.user_name, user.user_lastname, user.user_address, user.user_email, user.user_phonenumber], callback)
    },
//muokataan user:n tietoja
    update: function(user, user_id, callback){
        return db. query ("UPDATE user SET user_name=?, user_lastname=?, user_address=?, user_email=?, user_phonenumber=? WHERE user_id=?", 
            [user.user_name, user.user_lastname, user.user_address, user.user_email, user.user_phonenumber, user_id], callback)
    },

//poistetaan user kokonaan tietokannasta
    delete: function (user_id, callback){
        return db. query("DELETE FROM user WHERE user_id=?", [user_id], callback);
    }
}
//tämä antaa mahdollisuus käyttää tätä tiedostoa muissa tiedostoissa
module.exports=user;