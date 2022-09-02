
#include "val.h"

Value Value::operator+(const Value& op) const
{
    Value output;

    if(IsErr() || op.IsErr()){
        output = Value();
        return output;
    }
    else if(IsString() && op.IsString()){
        output = Value(GetString() + op.GetString());
        return output;
    }
    else if(IsInt() && op.IsReal()){
        output = Value(GetInt() + op.GetReal());
        return output;
    }
    else if(IsReal() && op.IsInt()){
        output = Value(GetReal() + op.GetInt());
        return output;
    }
    else if(IsReal() && op.IsReal()){
        output = Value(GetReal() + op.GetReal());
        return output;
    }
    else if(IsInt() && op.IsInt()){
        output = Value(GetInt() + op.GetInt());
        return output;
    }

    return output;

}

Value Value::operator-(const Value& op) const
{
    Value output;

    if(IsErr() || op.IsErr()){
        output = Value();
        return output;
    }
    else if(IsInt() && op.IsReal()){
        output = Value(GetInt() - op.GetReal());
        return output;
    }
    else if(IsReal() && op.IsInt()){
        output = Value(GetReal() - op.GetInt());
        return output;
    }
    else if(IsReal() && op.IsReal()){
        output = Value(GetReal() - op.GetReal());
        return output;
    }
    else if(IsInt() && op.IsInt()){
        output = Value(GetInt() - op.GetInt());
        return output;
    }

    return output;

}

Value Value::operator*(const Value& op) const
{
    Value output;

    if(IsErr() || op.IsErr()){
        output = Value();
        return output;
    }
    else if(IsInt() && op.IsReal()){
        output = Value(GetInt() * op.GetReal());
        return output;
    }
    else if(IsReal() && op.IsInt()){
        output = Value(GetReal() * op.GetInt());
        return output;
    }
    else if(IsReal() && op.IsReal()){
        output = Value(GetReal() * op.GetReal());
        return output;
    }
    else if(IsInt() && op.IsInt()){
        output = Value(GetInt() * op.GetInt());
        return output;
    }

    return output;

}

Value Value::operator/(const Value& op) const
{
    Value output;

    if(IsErr() || op.IsErr()){
        output = Value();
        return output;
    }
    else if(IsInt() && op.IsReal()){
        if(op.GetReal() == 0.0){
            output = Value();
        }
        else{
            output = Value(GetInt() / op.GetReal());
        }
        return output;
    }
    else if(IsReal() && op.IsInt()){
        if(op.GetInt() == 0){
            output = Value();
        }
        else{
            output = Value(GetReal() / op.GetInt());
        }
        return output;
    }
    else if(IsReal() && op.IsReal()){
        if(op.GetReal() == 0.0){
            output = Value();
        }
        else{
            output = Value(GetReal() / op.GetReal());
        }
        return output;
    }
    else if(IsInt() && op.IsInt()){
        if(op.GetInt() == 0){
            output = Value();
        }
        else{
            output = Value(GetInt() / op.GetInt());
        }
        return output;
    }

    return output;
}

Value Value::operator==(const Value& op) const
{
    Value output;

    if(IsErr() || op.IsErr()){
        output = Value();
        return output;
    }

    if(IsBool() && op.IsBool()){

        if(GetBool() == op.GetBool()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        }
    }

    if(IsString() && op.IsString()){

        if(GetString() == op.GetString()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        }
    }
       
    if(IsInt() && op.IsInt()){

        if(GetInt() == op.GetInt()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        }
        
    }

    if(IsReal() && op.IsReal()){

        if(GetReal() == op.GetReal()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        } 
    }

     if(IsReal() && op.IsInt()){
        if(GetReal() == op.GetInt()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        }
    }

    if(IsInt() && op.IsReal()){
        if(GetInt() == op.GetReal()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        }
    }

    return output;
}

Value Value::operator>(const Value& op) const
{
    Value output;

    if(IsErr() || op.IsErr()){
        output = Value();
        return output;
    }

    if(IsBool() && op.IsBool()){

        if(GetBool() > op.GetBool()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        }
    }

    if(IsString() && op.IsString()){

        if(GetString() > op.GetString()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        }
    }
       
    if(IsInt() && op.IsInt()){

        if(GetInt() > op.GetInt()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        }
        
    }

    if(IsReal() && op.IsReal()){

        if(GetReal() > op.GetReal()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        } 
    }

    if(IsReal() && op.IsInt()){
        if(GetReal() > op.GetInt()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        }
    }

    if(IsInt() && op.IsReal()){
        if(GetInt() > op.GetReal()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        }
    }

    return output;
}

Value Value::operator<(const Value& op) const
{
    Value output;

    if(IsErr() || op.IsErr()){
        output = Value();
        return output;
    }

    if(IsBool() && op.IsBool()){

        if(GetBool() < op.GetBool()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        }
    }

    if(IsString() && op.IsString()){

        if(GetString() < op.GetString()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        }
    }
       
    if(IsInt() && op.IsInt()){

        if(GetInt() < op.GetInt()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        }
        
    }

    if(IsReal() && op.IsReal()){

        if(GetReal() < op.GetReal()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        } 
    }

     if(IsReal() && op.IsInt()){
        if(GetReal() < op.GetInt()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        }
    }

    if(IsInt() && op.IsReal()){
        if(GetInt() < op.GetReal()){
            output = Value(true);
            return output;
        }
        else{
            output = Value(false);
            return output;
        }
    }

    return output;
}