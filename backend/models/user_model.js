const db = require('../routes/database');

const user={
    
//Get all users
    getAll: function(callback){
        return db. query ("SELECT * FROM user", callback);
    },
//Get single user by id
    getOne: function (user_id, callback){
        return db. query ("SELECT * FROM user WHERE user_id=?", [user_id], callback);
    },
 //Add new user 
    add: function (user, callback) {
        return db. query ("INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber) VALUES (?,?,?,?,?)",
        [user.user_name, user.user_lastname, user.user_address, user.user_email, user.user_phonenumber], callback)
    },

//Updated user data
    update: function(user, user_id, callback){
        return db. query ("UPDATE user SET user_address=?, user_email=?, user_phonenumber=? WHERE user_id=?", 
            [user.user_address, user.user_email, user.user_phonenumber, user_id], callback)
    },

//Delete user vy id
    delete: function (user_id, callback){
        return db. query("DELETE FROM user WHERE user_id=?", [user_id], callback);
    }
}

module.exports=user;